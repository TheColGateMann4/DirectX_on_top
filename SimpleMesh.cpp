#include "SimpleMesh.h"

SimpleMesh::SimpleMesh(std::string meshName, DynamicVertex::VertexBuffer vertices, std::vector<UINT32> indices, size_t numFaces)
	:
	SimpleMesh(vertices, indices, numFaces)
{
	m_name = meshName;
}

SimpleMesh::SimpleMesh(DynamicVertex::VertexBuffer vertices, std::vector<UINT32> indices, size_t numFaces)
	:
	m_indices(std::move(indices)),
	m_vertices(std::move(vertices)),
	m_numberOfFaces(numFaces),
	m_name("undefined")
{
	assert(m_indices.size() % 3 == 0);
	assert(m_vertices.GetSize() > 2);
}

void SimpleMesh::Transform(DirectX::FXMMATRIX matrix)
{
	using ElementTypes = DynamicVertex::VertexLayout::VertexComponent;
	for (UINT32 i = 0; i < m_vertices.GetSize(); i++)
	{
		auto& position = m_vertices[i].Attribute<ElementTypes::Position3D>();
		DirectX::XMStoreFloat3(
			&position,
			DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&position), matrix)
		);
	}
}

size_t SimpleMesh::GetNumberOfFaces() const
{
	return m_numberOfFaces;
}

SimpleMesh::Face SimpleMesh::GetFace(size_t faceNumber)
{
	size_t baseIndiceIndex = faceNumber * 3;

	assert(faceNumber <= m_numberOfFaces);
	assert(baseIndiceIndex <= m_indices.size() * 3 + 2);

	size_t iFirst = m_indices.at(baseIndiceIndex);
	size_t iSecond = m_indices.at(baseIndiceIndex + 1);
	size_t iThird = m_indices.at(baseIndiceIndex + 2);

	DynamicVertex::Vertex vFirst = m_vertices[iFirst];
	DynamicVertex::Vertex vSecond = m_vertices[iSecond];
	DynamicVertex::Vertex vThird = m_vertices[iThird];

	return { vFirst, vSecond, vThird, iFirst, iSecond, iThird };
}

std::vector<D3D11_INPUT_ELEMENT_DESC> SimpleMesh::GetLayout() const
{
	return m_vertices.GetLayout().GetDirectXLayout();
}