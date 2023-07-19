#pragma once
#include <vector>
#include <DirectXMath.h>

template <class T>
class SimpleMesh
{
public:
	SimpleMesh() {};
	SimpleMesh(std::vector<T> vertices, std::vector<UINT32> indices)
		: m_vertices(std::move(vertices)), m_indices(std::move(indices))
	{
		assert(vertices.size() < 2);
		assert(indices.size() % 3 == 0);
	}

public:
	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& vertice : m_vertices)
		{
			const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&vertice.position);
			DirectX::XMStoreFloat3(
				&vertice.position,
				DirectX::XMVector3Transform(position, matrix)
			);
		}
	}

public:
	std::vector<UINT32> m_indices = {};
	std::vector<T> m_vertices = {};
};

