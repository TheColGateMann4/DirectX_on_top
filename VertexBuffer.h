#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "Vertex.h"

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(GFX& gfx, const std::vector<V>& vertices)
		: stride(sizeof(V))
	{
		HRESULT hr;

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.CPUAccessFlags = NULL;
		vertexBufferDesc.MiscFlags = NULL;
		vertexBufferDesc.ByteWidth = (UINT32)(stride * vertices.size());
		vertexBufferDesc.StructureByteStride = stride;

		D3D11_SUBRESOURCE_DATA vertexBufferResourceData = {};
		vertexBufferResourceData.pSysMem = vertices.data();

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &pVertexBuffer));
	}
	VertexBuffer(GFX& gfx, const DynamicVertex::VertexBuffer& vertexBuffer)
		: stride((UINT32)vertexBuffer.GetLayout().GetByteSize())
	{
		HRESULT hr;

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.CPUAccessFlags = NULL;
		vertexBufferDesc.MiscFlags = NULL;
		vertexBufferDesc.ByteWidth = (UINT32)vertexBuffer.GetBytesSize();
		vertexBufferDesc.StructureByteStride = stride;

		D3D11_SUBRESOURCE_DATA vertexBufferResourceData = {};
		vertexBufferResourceData.pSysMem = vertexBuffer.GetData();

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &pVertexBuffer));
	}

	void Bind(GFX& gfx) noexcept override;

protected:
	UINT32 stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

