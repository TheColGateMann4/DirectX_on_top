#pragma once
#include "Includes.h"
#include "Bindable.h"

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
		vertexBufferDesc.ByteWidth = (UINT32)(sizeof(V) * vertices.size());
		vertexBufferDesc.StructureByteStride = sizeof(V);

		D3D11_SUBRESOURCE_DATA vertexBufferResourceData = {};
		vertexBufferResourceData.pSysMem = vertices.data();

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &pVertexBuffer));
	}

	VOID Bind(GFX& gfx) noexcept override;

protected:
	UINT32 stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

