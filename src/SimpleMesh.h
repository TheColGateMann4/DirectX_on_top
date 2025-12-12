#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Vertex.h"

class SimpleMesh
{
public:
	struct Face
	{
		DynamicVertex::Vertex vFirst, vSecond, vThird;
		size_t iFirst, iSecond, iThird;
	};

public:
	SimpleMesh() = default;
	SimpleMesh(std::string meshName, DynamicVertex::VertexBuffer vertices, std::vector<UINT32> indices, size_t numFaces = 0);
	SimpleMesh(DynamicVertex::VertexBuffer vertices, std::vector<UINT32> indices, size_t numFaces = 0);

	size_t GetNumberOfFaces() const;

	Face GetFace(size_t faceNumber);

public:
	void Transform(DirectX::FXMMATRIX matrix);

public:
	std::vector<D3D11_INPUT_ELEMENT_DESC> GetLayout() const;

public:
	std::vector<UINT32> m_indices = {};
	DynamicVertex::VertexBuffer m_vertices;
	size_t m_numberOfFaces = 0; // this functionality is optional
	std::string m_name;
};

