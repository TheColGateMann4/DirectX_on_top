#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Vertex.h"

class SimpleMesh
{
public:
	SimpleMesh() = default;
	SimpleMesh(DynamicVertex::VertexBuffer vertices, std::vector<UINT32> indices)
		: 
		m_indices(std::move(indices)),
		m_vertices(std::move(vertices))
	{
		assert(m_indices.size() % 3 == 0);
		assert(m_vertices.GetSize() > 2);
	}

public:
	void Transform(DirectX::FXMMATRIX matrix)
	{
		using ElementTypes = DynamicVertex::VertexLayout::VertexComponent;
		for (UINT32 i = 0; i < m_vertices.GetSize();i++)
		{
			auto& position = m_vertices[i].Attribute<ElementTypes::Position3D>();
			DirectX::XMStoreFloat3(
				&position,
				DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&position), matrix)
			);
		}
	}

public:
	std::vector<D3D11_INPUT_ELEMENT_DESC> GetLayout() const 
	{
		return m_vertices.GetLayout().GetDirectXLayout();
	}

public:
	std::vector<UINT32> m_indices = {};
	DynamicVertex::VertexBuffer m_vertices;

};

