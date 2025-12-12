#include "Vertex.h"
#include "ErrorMacros.h"

#define FOR_ALL_VERTEX_ELEMENTS(additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Position2D, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Position3D, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Tangent, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Bitangent, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Texture2D, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Normal, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Float3Color, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Float4Color, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::RGBAColor, additionalStatement)




DynamicVertex::VertexLayout::Element::Element(VertexComponent type, size_t offset)
	: 
	m_type(type), m_offset(offset) 
{}

size_t DynamicVertex::VertexLayout::Element::GetOffset() const
{
	return m_offset; 
}

size_t DynamicVertex::VertexLayout::Element::GetNextOffset() const noexcept(!IS_DEBUG)
{
	return m_offset + m_GetTypeSize(m_type); 
}

size_t DynamicVertex::VertexLayout::Element::GetType() const noexcept
{
	return m_type; 
}

size_t DynamicVertex::VertexLayout::Element::GetSize() const noexcept(!IS_DEBUG)
{
	return m_GetTypeSize(m_type); 
}

D3D11_INPUT_ELEMENT_DESC DynamicVertex::VertexLayout::Element::GetDirectXLayout() const noexcept(!IS_DEBUG)
{
#define STATEMENT(vertexComponent, additionalComponent) \
					case vertexComponent: \
					{ \
						return GenerateElementDesc<vertexComponent>(GetOffset()); \
					}

	switch (m_type)
	{
		FOR_ALL_VERTEX_ELEMENTS(void);
#undef STATEMENT
	}

	assert("invalid element type" && false);
	return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
}

const char* DynamicVertex::VertexLayout::Element::GetCode() const noexcept
{
	return GetVertexComponentVarible<DynamicVertex::VertexLayout::ComponentGrabberType::GetCode>::Get(m_type);
}

constexpr size_t DynamicVertex::VertexLayout::Element::m_GetTypeSize(DynamicVertex::VertexLayout::VertexComponent type) noexcept(!IS_DEBUG)
{
	return GetVertexComponentVarible<DynamicVertex::VertexLayout::ComponentGrabberType::GetSize>::Get(type);
}


/*						<-----------------------------VertexLayout----------------------------->						*/



const DynamicVertex::VertexLayout::Element& DynamicVertex::VertexLayout::GetByIndex(size_t i) const noexcept(!IS_DEBUG)
{
	return m_elements.at(i);
}

DynamicVertex::VertexLayout& DynamicVertex::VertexLayout::Append(DynamicVertex::VertexLayout::VertexComponent type) noexcept(!IS_DEBUG)
{
	m_elements.emplace_back(type, GetByteSize());
	return *this;
}

size_t DynamicVertex::VertexLayout::GetByteSize() const noexcept(!IS_DEBUG)
{
	return m_elements.empty() ? 0 : m_elements.back().GetNextOffset();
}

size_t DynamicVertex::VertexLayout::GetStructureSize() const noexcept(!IS_DEBUG)
{
	return GetByteSize() / GetElementCount();
}

size_t DynamicVertex::VertexLayout::GetElementCount() const noexcept
{
	return m_elements.size();
}

std::vector<D3D11_INPUT_ELEMENT_DESC> DynamicVertex::VertexLayout::GetDirectXLayout() const noexcept(!IS_DEBUG)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> result;
	result.reserve(GetElementCount());

	for (auto& element : m_elements)
		result.push_back(element.GetDirectXLayout());

	return result;
}

std::string DynamicVertex::VertexLayout::GetUID() const
{
	std::string result = {};

	for (auto& element : m_elements)
		result += element.GetCode();

	return result;
}

size_t DynamicVertex::VertexLayout::GetIndexOfElement(VertexLayout::VertexComponent elementType) const
{
	for (size_t i = 0; i < m_elements.size(); i++)
		if (m_elements[i].GetType() == elementType)
			return i;

	std::string errorString = "Layout element type could not be found. Element type was: \"";
	errorString += std::to_string(static_cast<size_t>(elementType));
	errorString += "\".";
	THROW_INTERNAL_ERROR(errorString.c_str());
}


/*						<-----------------------------Vertex----------------------------->						*/


DynamicVertex::Vertex::Vertex(char* pData, const DynamicVertex::VertexLayout& layout)
	: m_pData(pData), m_layout(layout)
{
	assert(m_pData != nullptr);
}


/*						<-----------------------------ConstVertex----------------------------->						*/


DynamicVertex::ConstVertex::ConstVertex(const DynamicVertex::Vertex& vertex) noexcept(!IS_DEBUG)
	:	
	m_vertex(vertex)
{}


/*						<-----------------------------VertexBuffer----------------------------->						*/


DynamicVertex::VertexBuffer::VertexBuffer(DynamicVertex::VertexLayout layout) noexcept(!IS_DEBUG)
	: m_layout(std::move(layout)) {}

const char* DynamicVertex::VertexBuffer::GetConstData() const noexcept(!IS_DEBUG)
{
	return m_buffer.data();
}
char* DynamicVertex::VertexBuffer::GetData() noexcept(!IS_DEBUG)
{
	return m_buffer.data();
}

const DynamicVertex::VertexLayout& DynamicVertex::VertexBuffer::GetLayout() const noexcept
{
	return m_layout;
}

//size in vertices
size_t DynamicVertex::VertexBuffer::GetSize() const noexcept(!IS_DEBUG)
{
	return m_buffer.size() / m_layout.GetByteSize();
}

//size in bytes
size_t DynamicVertex::VertexBuffer::GetBytesSize() const noexcept(!IS_DEBUG)
{
	return m_buffer.size();
}

// might be unsafe
void DynamicVertex::VertexBuffer::Emplace_Back_Empty(size_t numberOfVertices) noexcept(!IS_DEBUG)
{
	const size_t layoutSize = m_layout.GetByteSize();
	const size_t addedSize = layoutSize * numberOfVertices;

	m_buffer.resize(m_buffer.size() + addedSize);
	memset(static_cast<void*>((m_buffer.data() + m_buffer.size()) - addedSize), '\0', addedSize);
}

void DynamicVertex::VertexBuffer::MakeFromMesh(const aiMesh& mesh)
{
	size_t numberOfVertices = mesh.mNumVertices;

	this->Emplace_Back_Empty(numberOfVertices);

	for (size_t i = 0; i < numberOfVertices; i++)
	{
		size_t currentElementIndex = 0;
		for (const auto& element : m_layout.m_elements)
		{
#define STATEMENT(vertexComponent, additionalComponent) \
					case vertexComponent:	\
					{	\
						(*this)[i].SetAttribute<vertexComponent>(	\
							this->GetData() + this->GetLayout().GetByteSize() * i + this->GetLayout().GetByIndex(currentElementIndex).GetOffset(),	\
							::DynamicVertex::VertexLayout::Map<vertexComponent>::GetData(mesh, i, vertexScale)	\
						);	\
						break;	\
					}

			switch (element.GetType())
			{

				FOR_ALL_VERTEX_ELEMENTS(void);
#undef STATEMENT

			}
			currentElementIndex++;
		}
	}
}

void DynamicVertex::VertexBuffer::SetScale(float scale)
{
	vertexScale = scale;
}



//Vertex stuff
DynamicVertex::Vertex DynamicVertex::VertexBuffer::Back() noexcept(!IS_DEBUG)
{
	assert(m_buffer.size() != 0);
	return Vertex{
		m_buffer.data() + m_buffer.size() - m_layout.GetByteSize(),
		m_layout
	};
}

DynamicVertex::Vertex DynamicVertex::VertexBuffer::Front() noexcept(!IS_DEBUG)
{
	assert(m_buffer.size() / m_layout.GetByteSize() != 0);
	return Vertex{
		m_buffer.data(),
		m_layout
	};
}

DynamicVertex::Vertex DynamicVertex::VertexBuffer::operator[](size_t i) noexcept(!IS_DEBUG)
{
	assert(i < m_buffer.size() / m_layout.GetByteSize());
	return Vertex{ m_buffer.data() + (i * m_layout.GetByteSize()) , m_layout };
}




//Const Vertex Stuff
DynamicVertex::ConstVertex DynamicVertex::VertexBuffer::Back() const noexcept(!IS_DEBUG)
{
	return const_cast<VertexBuffer*>(this)->Back();
}

DynamicVertex::ConstVertex DynamicVertex::VertexBuffer::Front() const noexcept(!IS_DEBUG)
{
	return const_cast<VertexBuffer*>(this)->Front();
}

DynamicVertex::ConstVertex DynamicVertex::VertexBuffer::operator[](size_t i) const noexcept(!IS_DEBUG)
{
	return const_cast<VertexBuffer&>(*this)[i];
}


#undef FOR_ALL_VERTEX_ELEMENTS