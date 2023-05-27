#pragma once
#include "Includes.h"
#include "ErrorHandling.h"

template<class T>
class Mesh
{
public:
	Mesh() = default;
	Mesh(std::vector<T> vertices, std::vector<UINT32> indices)
		: m_vertices(std::move(vertices)), m_indices(std::move(indices))
	{
		THROW_INTERNAL_ERROR("A mesh needs at least two vertices.", m_vertices.size() < 2);
		THROW_INTERNAL_ERROR("Indices must create triangles; indices % 3 != 0.", m_indices.size() % 3 != 0);
	}

public:
	VOID Transform(DirectX::XMMATRIX martix)
	{
		for (auto& v : m_vertices)
		{
			const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&v.position);

			DirectX::XMStoreFloat3(
				&v.position,
				DirectX::XMVector3Transform(position, martix)
			);
		}
	}

public:
	std::vector<T> m_vertices;
	std::vector<UINT32> m_indices;
}; 