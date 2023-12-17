#include "DynamicConstantBuffer.h"
#include <string>
#include <cmath>
#include <span>

#define FOR_ALL_TYPES STATEMENT(Float) \
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

		result += GetElementSize(element.second->GetType());
	}

	assert(false, "could not have found element in buffer layout");
	return 0;
}

const DynamicConstantBuffer::BufferLayout& DynamicConstantBuffer::BufferLayout::GetFinished(size_t* newSize, std::vector<size_t>* addedSpacesOffsets)
{

	assert(m_elements.size() != 0, "Trying to finalize layout with vector length 0");


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

		size_t elementSize = GetElementSize(m_elements.at(i).second->GetType());

		if (accumulatedSize + elementSize > 16 && accumulatedSize != 0)
		{
			size_t spaceToAdd = 16 - accumulatedSize;

			// doing it now since spaceToAdd will change in for loop
			// substracting 1 since we want to still loop throught the element after the padding
			size_t indexOfNextNumber = i + ((spaceToAdd / 4) - 1);


			for (; spaceToAdd > 0; spaceToAdd -= 4) 
			{
				std::string paddingNumberString = std::to_string(paddingNumber++);
				m_elements.insert(m_elements.begin() + i, { newElementName + paddingNumberString, std::make_shared<LayoutElement>(DynamicConstantBuffer::DataType::Float) });
				
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

		m_elements.push_back({ newElementName + paddingNumberString, std::make_shared<LayoutElement>(DynamicConstantBuffer::DataType::Float) });
		
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


		if (elementType == DynamicConstantBuffer::Struct)
		{
			const auto& structData = static_cast<DynamicConstantBuffer::AdditionalElements::StructData&>(*element.second->m_additionalData);
			for (const auto& structElement : structData.data)
				result += GetElementSize(structElement.second->GetType());
		}
		else if (elementType == DynamicConstantBuffer::Array)
		{
			const auto& arrayData = static_cast<DynamicConstantBuffer::AdditionalElements::ArrayData&>(*element.second->m_additionalData);
			result += GetElementSize(arrayData.type) * arrayData.size;
		}
		else
		{
			result += GetElementSize(elementType);
		}
	}
	return result;
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
	assert(identificatorSize / 2 == floor(identificatorSize / 2), "incorrect identificator size");

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
			assert(endOfStructIdentificator != std::string::npos, "Struct closing branch could not be found, identificator got corrupted");

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
			assert(endOfArrayIdentificator != std::string::npos, "Array closing branch could not be found, identificator got corrupted");

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

		assert(false, "identificator " + codestring + " could not be identified");


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

	assert(false, std::string(elementName + "element name could not be found in buffer").c_str());
	return nullptr;
}


const DynamicConstantBuffer::LayoutElement* DynamicConstantBuffer::BufferLayout::operator[](const char* elementName) const noexcept
{
	for (const auto& element : m_elements)
	{
		if (element.first == elementName)
			return element.second.get();
	}

	assert(false, std::string(elementName + "element name could not be found in buffer").c_str());
	return nullptr;
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

	static_assert("GetElementSize() could not match DataType with any known one");
	return NULL;
}




DynamicConstantBuffer::LayoutElement::LayoutElement(DynamicConstantBuffer::DataType elementType)
{
	m_type = elementType;

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
	assert(this->m_type != Struct, "Cannot add places to not Struct element, element was type " + this->m_type);

	auto& additionalData = static_cast<DynamicConstantBuffer::AdditionalElements::StructData&>(*m_additionalData);

	for (auto& structElement : additionalData.data)
	{
		if (structElement.first == elementName)
			return structElement.second.get();
	}

	assert(false, std::string(elementName + "element name could not be found in struct").c_str());
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

void DynamicConstantBuffer::BufferData::MakeFinished()
{
	size_t bytesAdded = 0;
	size_t possibleSizeWithoutPaddingAtTheEnd = m_size;// later, when adding padding data, we need to know our end of data to move
	std::vector<size_t> addedSpacesVector;

	(void)m_layout.GetFinished(&bytesAdded, &addedSpacesVector);

	if (bytesAdded == 0)
		return;

	ResizeBuffer(m_size + bytesAdded);

	if (m_size == 48)
		std::cout << "co";

	for (const auto& addedSpace : addedSpacesVector)
	{
		size_t sourceSizeToMove = possibleSizeWithoutPaddingAtTheEnd - (addedSpace * 4);

		// multiplying by 4 because we want to get size of floats, and plus 4 since we are moving memory by 1 float size
		size_t destinationSize = m_size - ((addedSpace * 4) + 4); 

		void* src = static_cast<char*>(m_pDataBuffer) + (addedSpace * 4);
		void* dest = static_cast<char*>(src) + 4;

		memmove_s(dest, destinationSize, src, sourceSizeToMove);

		possibleSizeWithoutPaddingAtTheEnd += 4;
	}
}

DynamicConstantBuffer::BufferLayout& DynamicConstantBuffer::BufferData::GetMutableLayout()
{
	return m_layout;
}

const DynamicConstantBuffer::BufferLayout& DynamicConstantBuffer::BufferData::GetConstLayout() const
{
	return m_layout;
}

void DynamicConstantBuffer::BufferData::Update(const DynamicConstantBuffer::BufferLayout layout)
{
	m_layout = layout;
	m_size = DynamicConstantBuffer::BufferLayout::GetLayoutSize(layout);

	m_pDataBuffer = new char[m_size];
}

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