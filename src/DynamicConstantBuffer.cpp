#include "DynamicConstantBuffer.h"
#include <imgui.h>

#define FOR_ALL_TYPES \
	STATEMENT(Padding) \
	STATEMENT(Int) \
	STATEMENT(Float) \
	STATEMENT(Float2) \
	STATEMENT(Float3) \
	STATEMENT(Float4) \
	STATEMENT(Matrix) \
	STATEMENT(Bool) 

DynamicConstantBuffer::BufferLayout::BufferLayout(const char* bufferIdentificator)
{
	MakeBufferFromIdentificator(bufferIdentificator);
}

DynamicConstantBuffer::BufferLayout::BufferLayout()
{
	m_elements = {};
}


DynamicConstantBuffer::BufferLayout::~BufferLayout()
{
	if (m_elements.empty())
		return;

	m_elements.clear();
}


bool DynamicConstantBuffer::BufferLayout::isExisting(const char* elementName)
{
	for (auto& element : m_elements)
		if (element.first == elementName)
			return true;
	return false;
}

size_t DynamicConstantBuffer::BufferLayout::GetOffsetOfElement(const char* elementName) const
{
	size_t result = 0;

	for (const auto& element : m_elements)
	{
		if(element.first == elementName)
			return result;

		result += GetElementSize(element.first.c_str());
	}

	assert(false && "could not have found element in buffer layout");
	return 0;
}

size_t DynamicConstantBuffer::BufferLayout::GetIndexOfElement(const char* elementName) const
{
	size_t result = 0;

	for (const auto& element : m_elements)
	{
		if (element.first == elementName)
			return result;

		result++;
	}

	assert(false && "could not have found element in buffer layout");
	return 0;
}

const DynamicConstantBuffer::BufferLayout& DynamicConstantBuffer::BufferLayout::GetFinished(size_t* newSize, std::vector<size_t>* addedSpacesOffsets)
{

	assert(m_elements.size() != 0 && "Trying to finalize layout with vector length 0");


	std::string newElementName = "padding";
	size_t paddingNumber = 0;

	size_t accumulatedSize = 0;
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		if (m_elements.at(i).first.find("padding", 0) == 0)
		{
			accumulatedSize += 4;
			continue;
		}

		size_t elementSize = GetElementSize(m_elements.at(i).first.c_str());

		if (accumulatedSize + elementSize > 16 && accumulatedSize != 0)
		{
			size_t spaceToAdd = 16 - accumulatedSize;

			// doing it now since spaceToAdd will change in for loop
			// substracting 1 since we want to still loop throught the element after the padding
			size_t indexOfNextNumber = i + ((spaceToAdd / 4) - 1);


			for (; spaceToAdd > 0; spaceToAdd -= 4) 
			{
				std::string paddingNumberString = std::to_string(paddingNumber++);
				m_elements.insert(m_elements.begin() + i, { newElementName + paddingNumberString, std::make_shared<LayoutElement>(DynamicConstantBuffer::DataType::Padding) });
				
				if (newSize != nullptr)
					*newSize += 4;

				if (addedSpacesOffsets != nullptr)
					addedSpacesOffsets->push_back(i);
			}

			i = indexOfNextNumber;
		}
		else if(accumulatedSize + elementSize < 16)
		{
			accumulatedSize += elementSize;
			continue;
		}

		//if its equal or higher we want to set it to 0

		accumulatedSize = 0;
	}

	size_t currentSize = GetLayoutSize(*this);

 	size_t missingSpaceInBuffer = (currentSize % 16);

	if (missingSpaceInBuffer == 0)
		return *this;

	missingSpaceInBuffer = 16 - missingSpaceInBuffer;

	for (size_t i = 0; i < missingSpaceInBuffer / 4; i++)
	{
		//the padding won't ever need to be a two digit number, so we can do it like this
		std::string paddingNumberString = std::to_string(paddingNumber++);

		m_elements.push_back({ newElementName + paddingNumberString, std::make_shared<LayoutElement>(DynamicConstantBuffer::DataType::Padding) });
		
		if (newSize != nullptr)
			*newSize += 4;
	}


	return *this;
}


size_t DynamicConstantBuffer::BufferLayout::GetLayoutSize(const DynamicConstantBuffer::BufferLayout& layout)
{
	size_t result = 0;
	for (const auto& element : layout.m_elements)
	{

		DynamicConstantBuffer::DataType elementType = element.second->GetType();

		if (elementType == DynamicConstantBuffer::Empty)
			continue;


		result += layout.GetElementSize(element.first.c_str());
	}
	return result;
}


void DynamicConstantBuffer::BufferLayout::SetArrayAttributes(const char* arrayName, DynamicConstantBuffer::DataType type, size_t size)
{
	assert((type != Array && type != Empty && type != Struct) && "invalid array type tried to be set");

	size_t elementIndex = GetIndexOfElement(arrayName);

	LayoutElement* arrayElement = m_elements.at(elementIndex).second.get();

	assert(arrayElement->GetType() == Array && "used SetArrayType() funtion on non array element");

	 AdditionalElements::AdditionalDataBase* dataBase = arrayElement->m_additionalData.get();

	AdditionalElements::ArrayData* arrayData = static_cast<AdditionalElements::ArrayData*>(dataBase);

	arrayData->type = type;
	arrayData->size = size;
}


std::string DynamicConstantBuffer::BufferLayout::GetIdentificator() const
{
	std::string result = {};
	for (const auto& element : m_elements)
	{
		DynamicConstantBuffer::DataType elementType = element.second->GetType();

		if (elementType == DynamicConstantBuffer::Empty)
			continue;


		if (elementType == DynamicConstantBuffer::Struct)
		{
			const auto& structData = static_cast<DynamicConstantBuffer::AdditionalElements::StructData&>(*element.second->m_additionalData);

			result += "{";

			for (const auto& structElement : structData.data)
				result += GetElementCode(structElement.second->GetType());

			result += "}";
		}
		else if (elementType == DynamicConstantBuffer::Array)
		{
			const auto& arrayData = static_cast<DynamicConstantBuffer::AdditionalElements::ArrayData&>(*element.second->m_additionalData);

			result += "(";

			result += GetElementCode(arrayData.type);
			result += std::to_string(arrayData.size);

			result += ")";
		}
		else
		{
			result += GetElementCode(elementType);
		}
	}
	return std::move(result);
}

void DynamicConstantBuffer::BufferLayout::MakeBufferFromIdentificator(const char* identificator, bool calledByFunction, DynamicConstantBuffer::AdditionalElements::StructData* vectorData)
{
	if(!calledByFunction)
		this->m_elements.clear();

	// if the function is called to fill struct it will gently refer to that vector
	auto& localVector = (calledByFunction ? vectorData->data : this->m_elements);


	const size_t identificatorSize = strlen(identificator);
	assert((identificatorSize / 2 == floor(identificatorSize / 2)) && "incorrect identificator size");

	auto ProcessDatatype = [](const DynamicConstantBuffer::DataType type, auto& localVector) // std::vector<std::pair<std::string, LayoutElement*>>
		{
			std::string elementName = "element";
			elementName += std::to_string(localVector.size());

			localVector.push_back({ elementName, std::make_shared<LayoutElement>(type)});
		};

	std::string_view identificatorView = identificator;

	for (size_t i = 0; i < identificatorSize; i = i + 2)
	{
		std::string codestring = { identificator[i] };

		if (codestring[0] == '{')
		{
			size_t endOfStructIdentificator = identificatorView.find('}');
			assert(endOfStructIdentificator != std::string::npos && "Struct closing branch could not be found, identificator got corrupted");

			ProcessDatatype(DynamicConstantBuffer::DataType::Struct, localVector);

			if ((endOfStructIdentificator - 1) - i != 0)
			{
				std::string identificatorForFunction(endOfStructIdentificator - i, '\0');

				memmove_s(identificatorForFunction.data(), endOfStructIdentificator - i, identificator + i, endOfStructIdentificator - i);

				auto structData = static_cast<DynamicConstantBuffer::AdditionalElements::StructData*>(localVector.back().second->m_additionalData.get());

				this->MakeBufferFromIdentificator(identificatorForFunction.c_str(), true, structData);
			}
		}
		else if (codestring[0] == '(')
		{
			size_t endOfArrayIdentificator = identificatorView.find(')');
			assert(endOfArrayIdentificator != std::string::npos && "Array closing branch could not be found, identificator got corrupted");

			ProcessDatatype(DynamicConstantBuffer::DataType::Array, localVector);


		}

		codestring += identificator[i + 1];

#define STATEMENT(macro_element)	\
		if (codestring == DynamicConstantBuffer::DataTypeMap<DynamicConstantBuffer::DataType::macro_element>::code)	\
		{	\
			ProcessDatatype(DynamicConstantBuffer::DataType::macro_element, localVector);	\
			continue;	\
		}


		FOR_ALL_TYPES

		assert(false && "identificator could not be identified");


#undef STATEMENT
	}
}


DynamicConstantBuffer::BufferLayout DynamicConstantBuffer::BufferLayout::operator=(const char* elementIdentificator) noexcept
{
	if(elementIdentificator != nullptr) // if nullptr then user wants to default initialize the layout
		MakeBufferFromIdentificator(elementIdentificator);

	return *this;
}


DynamicConstantBuffer::BufferLayout DynamicConstantBuffer::BufferLayout::operator=(const DynamicConstantBuffer::BufferLayout& layout) noexcept
{
	this->m_elements = layout.m_elements;
	return *this;
}


DynamicConstantBuffer::LayoutElement* DynamicConstantBuffer::BufferLayout::operator[](const char* elementName) noexcept
{
	for (auto& element : m_elements)
	{
		if (element.first == elementName)
			return element.second.get();
	}

	assert(false && "element name could not be found in buffer");
	return nullptr;
}


const DynamicConstantBuffer::LayoutElement* DynamicConstantBuffer::BufferLayout::operator[](const char* elementName) const noexcept
{
	for (const auto& element : m_elements)
	{
		if (element.first == elementName)
			return element.second.get();
	}

	assert(false && "element name could not be found in buffer");
	return nullptr;
}


size_t DynamicConstantBuffer::BufferLayout::GetElementSize(const char* elementName) const
{
	LayoutElement* layoutElement = m_elements.at(GetIndexOfElement(elementName)).second.get();
	DynamicConstantBuffer::DataType dataType = layoutElement->GetType();

	if (dataType == DynamicConstantBuffer::DataType::Struct)
	{
		const auto& structData = static_cast<DynamicConstantBuffer::AdditionalElements::StructData&>(*layoutElement->m_additionalData);

		size_t result = 0;
		for (const auto& structElement : structData.data)
			result += GetElementSize(structElement.second->GetType());
		return result;
	}
	else if (dataType == DynamicConstantBuffer::DataType::Array)
	{
		const auto& arrayData = static_cast<DynamicConstantBuffer::AdditionalElements::ArrayData&>(*layoutElement->m_additionalData);
		return GetElementSize(arrayData.type) * arrayData.size;
	}
	
	return GetElementSize(dataType);
}

size_t DynamicConstantBuffer::BufferLayout::GetElementSize(DynamicConstantBuffer::DataType type)
{
#define STATEMENT(macro_element)	\
	case DynamicConstantBuffer::macro_element:	\
	{	\
		return DynamicConstantBuffer::DataTypeMap<DynamicConstantBuffer::macro_element>().hlslsize;	\
	}

	switch (type)
	{
		FOR_ALL_TYPES
	}
#undef STATEMENT

	static_assert("GetElementSize() could not match DataType with any known one");
	return NULL;
}


const char* DynamicConstantBuffer::BufferLayout::GetElementCode(DynamicConstantBuffer::DataType type)
{
#define STATEMENT(macro_element)	\
	case DynamicConstantBuffer::macro_element:	\
	{	\
		return DynamicConstantBuffer::DataTypeMap<DynamicConstantBuffer::macro_element>().code;	\
	}

	switch (type)
	{
		FOR_ALL_TYPES
	}
#undef STATEMENT

	static_assert("GetElementCode() could not match DataType with any known one");
	return NULL;
}


DynamicConstantBuffer::LayoutElement::LayoutElement(DynamicConstantBuffer::DataType elementType, ImguiAdditionalInfo::ImguiInfo* imguiInfo)
{
	m_type = elementType;

	switch (elementType)
	{
		case Int:
		{
			if (ImguiAdditionalInfo::ImguiIntInfo* arrr = dynamic_cast<ImguiAdditionalInfo::ImguiIntInfo*>(imguiInfo))
				m_imguiInfo = std::make_unique<ImguiAdditionalInfo::ImguiIntInfo>(*arrr);
			else
				m_imguiInfo = std::make_unique<ImguiAdditionalInfo::ImguiIntInfo>();

			break;
		}
		case Float:
		{
			if (ImguiAdditionalInfo::ImguiFloatInfo* arrr = dynamic_cast<ImguiAdditionalInfo::ImguiFloatInfo*>(imguiInfo))
				m_imguiInfo = std::make_unique<ImguiAdditionalInfo::ImguiFloatInfo>(*arrr);
			else
				m_imguiInfo = std::make_unique<ImguiAdditionalInfo::ImguiFloatInfo>();

			break;
		}
		case Float3:
		case Float4:
		{
			if (ImguiAdditionalInfo::ImguiColorInfo* arrr = dynamic_cast<ImguiAdditionalInfo::ImguiColorInfo*>(imguiInfo))
				m_imguiInfo = std::make_unique<ImguiAdditionalInfo::ImguiColorInfo>(*arrr);
			else
				m_imguiInfo = std::make_unique<ImguiAdditionalInfo::ImguiColorInfo>();

			break;
		}
		default:
		{
			m_imguiInfo = std::make_unique<ImguiAdditionalInfo::ImguiInfo>();

			break;
		}
	}

	if (m_type == Struct)
		m_additionalData = std::make_unique<DynamicConstantBuffer::AdditionalElements::StructData>();
	else if (m_type == Array)
		m_additionalData = std::make_unique<DynamicConstantBuffer::AdditionalElements::ArrayData>();
	else
		m_additionalData = std::make_unique<DynamicConstantBuffer::AdditionalElements::AdditionalDataBase>();
}


DynamicConstantBuffer::LayoutElement::LayoutElement()
{
	m_additionalData = std::make_unique<DynamicConstantBuffer::AdditionalElements::AdditionalDataBase>();
}


DynamicConstantBuffer::LayoutElement::~LayoutElement()
{
	if (this->m_type != DynamicConstantBuffer::DataType::Struct)
		return;

	auto& additionalData = static_cast<DynamicConstantBuffer::AdditionalElements::StructData&>(*m_additionalData);

	for (auto& element : additionalData.data)
		delete element.second.get();

	additionalData.data.clear();
}


const DynamicConstantBuffer::LayoutElement* DynamicConstantBuffer::LayoutElement::operator[](const char* elementName) noexcept
{
	assert(this->m_type != Struct && "Cannot add places to not Struct element, element was type ");

	auto& additionalData = static_cast<DynamicConstantBuffer::AdditionalElements::StructData&>(*m_additionalData);

	for (auto& structElement : additionalData.data)
	{
		if (structElement.first == elementName)
			return structElement.second.get();
	}

	assert(false && "element name could not be found in struct");
	return nullptr;
}










DynamicConstantBuffer::BufferData::BufferData(const BufferLayout& layout)
{
	m_layout = layout;
	m_size = DynamicConstantBuffer::BufferLayout::GetLayoutSize(layout);

	m_pDataBuffer = new char[m_size];
}

DynamicConstantBuffer::BufferData::BufferData(const BufferData& bufferToAssign)
{
	*this = bufferToAssign;
}

DynamicConstantBuffer::BufferData::BufferData()
{
	m_layout = {};
}

DynamicConstantBuffer::BufferData::~BufferData()
{
	if (m_pDataBuffer != nullptr)
		delete[] m_pDataBuffer;
}

bool DynamicConstantBuffer::BufferData::MakeImguiMenu()
{
	bool changed = false;

	auto checkChanged = [&changed](bool returnFromStatement) mutable
		{
			changed = changed || returnFromStatement;
		};

	for (auto& element : this->m_layout.GetVector())
	{
		const DataType elementType = element.second->GetType();
		const char* elementName = element.first.c_str();
		const ImguiAdditionalInfo::ImguiInfo* imguiInfo = element.second->GetImGuiInfo();

		if(elementType != Bool && elementType != Padding && elementType != Empty)
			assert(imguiInfo != nullptr && "Imgui propeties weren't defined for given element");


		switch (elementType)
		{
			case Empty:
			case Padding:
			{
				continue;
			}
			case Int:
			{
				const ImguiAdditionalInfo::ImguiIntInfo* imguiIntInfo = static_cast<const ImguiAdditionalInfo::ImguiIntInfo*>(imguiInfo);
				checkChanged(ImGui::SliderInt(elementName, this->GetElementPointerValue<DynamicConstantBuffer::DataType::Int>(elementName), imguiIntInfo->v_min, imguiIntInfo->v_max, imguiIntInfo->format.c_str(), imguiIntInfo->flags));
				break;
			}
			case Float:
			{
				const ImguiAdditionalInfo::ImguiFloatInfo* imguiFloatInfo = static_cast<const ImguiAdditionalInfo::ImguiFloatInfo*>(imguiInfo);
				checkChanged(ImGui::SliderFloat(elementName, this->GetElementPointerValue<DynamicConstantBuffer::DataType::Float>(elementName), imguiFloatInfo->v_min, imguiFloatInfo->v_max, imguiFloatInfo->format.c_str(), imguiFloatInfo->flags));
				break;
			}
			case Float3:
			{
				const ImguiAdditionalInfo::ImguiColorInfo* imguiColorInfo = static_cast<const ImguiAdditionalInfo::ImguiColorInfo*>(imguiInfo);
				checkChanged(ImGui::ColorPicker3(elementName, reinterpret_cast<float*>(this->GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>(elementName)), imguiColorInfo->flags));
				break;
			}
			case Float4:
			{
				const ImguiAdditionalInfo::ImguiColorInfo* imguiColorInfo = static_cast<const ImguiAdditionalInfo::ImguiColorInfo*>(imguiInfo);
				checkChanged(ImGui::ColorPicker4(elementName, reinterpret_cast<float*>(this->GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>(elementName)), imguiColorInfo->flags));
				break;
			}
			case Bool:
			{
				checkChanged(ImGui::Checkbox(elementName, (bool*)this->GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>(elementName)));
				break;
			}

			default:
			{
				assert(false && "Buffer element was not defined for imgui display");
			}
		}
	}

	return changed;
}

bool DynamicConstantBuffer::BufferData::ElementExists(const char* elementName)
{
	return m_layout.isExisting(elementName);
}

void DynamicConstantBuffer::BufferData::MakeFinished()
{
	size_t bytesAdded = 0;
	size_t possibleSizeWithoutPaddingAtTheEnd = m_size;// later, when adding padding data, we need to know our end of data to move
	std::vector<size_t> addedSpacesVector;

	(void)m_layout.GetFinished(&bytesAdded, &addedSpacesVector);

	if (bytesAdded == 0)
		return;

	ResizeBuffer(m_size + bytesAdded);

	for (size_t i = 0; i < addedSpacesVector.size(); i++)
	{
		size_t currentAddedSpacePosition = addedSpacesVector.at(i) + i;

		size_t sourceSizeToMove = possibleSizeWithoutPaddingAtTheEnd - (currentAddedSpacePosition * 4);

		// multiplying by 4 because we want to get size of floats, and plus 4 since we are moving memory by 1 float size
		size_t destinationSize = m_size - ((currentAddedSpacePosition * 4) + 4);

		void* src = static_cast<char*>(m_pDataBuffer) + (currentAddedSpacePosition * 4);
		void* dest = static_cast<char*>(src) + 4;

		memmove_s(dest, destinationSize, src, sourceSizeToMove);

		possibleSizeWithoutPaddingAtTheEnd += 4;
	}
}

const DynamicConstantBuffer::BufferLayout& DynamicConstantBuffer::BufferData::GetLayout() const
{
	return m_layout;
}

void DynamicConstantBuffer::BufferData::Update(const DynamicConstantBuffer::BufferLayout layout)
{
	m_layout = layout;
	m_size = DynamicConstantBuffer::BufferLayout::GetLayoutSize(layout);

	m_pDataBuffer = new char[m_size];
}

#ifdef _DEBUG
void DynamicConstantBuffer::BufferData::DebugLayout() const
{
	const auto& constLayout = GetLayout();
	const auto& layoutVector = constLayout.GetVector();

	for (const auto& layoutElement : layoutVector)
	{
		size_t elementOffset = constLayout.GetOffsetOfElement(layoutElement.first.c_str());
		std::cout << "name: " << layoutElement.first << " offsets: " << elementOffset << " size: " << m_layout.GetElementSize(layoutElement.first.c_str()) << '\n';
	}
}
#else
void DynamicConstantBuffer::BufferData::DebugLayout() const
{

}
#endif

void* DynamicConstantBuffer::BufferData::GetBytes() const
{
	return m_pDataBuffer;
}

size_t DynamicConstantBuffer::BufferData::GetSize() const
{
	return m_size;
}

void DynamicConstantBuffer::BufferData::ResizeBuffer(size_t newSize)
{
	void* newBuffer = new char[newSize];

	if (m_pDataBuffer != nullptr)
	{
		memcpy_s(newBuffer, newSize, m_pDataBuffer, m_size);

		delete[] m_pDataBuffer;
	}

	m_size = newSize;
	m_pDataBuffer = newBuffer;
}