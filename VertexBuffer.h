#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "BindableList.h"
#include "Vertex.h"

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(GFX& gfx, const std::vector<V>& vertices)
		: m_stride(sizeof(V))
	{
		HRESULT hr;

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.CPUAccessFlags = NULL;
		vertexBufferDesc.MiscFlags = NULL;
		vertexBufferDesc.ByteWidth = (UINT32)(m_stride * vertices.size());
		vertexBufferDesc.StructureByteStride = m_stride;

		D3D11_SUBRESOURCE_DATA vertexBufferResourceData = {};
		vertexBufferResourceData.pSysMem = vertices.data();

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &pVertexBuffer));
	}

	VertexBuffer(GFX& gfx, const DynamicVertex::VertexBuffer& vertexBuffer)
		:
		VertexBuffer(gfx, "UNKNOWN", vertexBuffer)
	{}

	VertexBuffer(GFX& gfx, const std::string bufferUID, const DynamicVertex::VertexBuffer& vertexBuffer)
		: 
		m_stride((UINT32)vertexBuffer.GetLayout().GetStructureSize()),
		m_bufferUID(bufferUID)
	{
		HRESULT hr;

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.CPUAccessFlags = NULL;
		vertexBufferDesc.MiscFlags = NULL;
		vertexBufferDesc.ByteWidth = (UINT32)vertexBuffer.GetBytesSize();
		vertexBufferDesc.StructureByteStride = m_stride;

		D3D11_SUBRESOURCE_DATA vertexBufferResourceData = {};
		vertexBufferResourceData.pSysMem = vertexBuffer.GetData();

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &pVertexBuffer));
	}

	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<VertexBuffer> GetBindable(GFX& gfx, const std::string bufferUID, const DynamicVertex::VertexBuffer& vertexBuffer)
	{
		assert(bufferUID != "UNKNOWN");
		return BindableList::GetBindable<VertexBuffer>(gfx, bufferUID, vertexBuffer);
	}

	template <class ...Params>
	static std::string GetUID(const std::string bufferUID, Params&& ...params)
	{
		return GenerateUID(bufferUID);
	};

	std::string GetUID() const noexcept override
	{
		return GenerateUID(m_bufferUID);
	};

private:
	static std::string GenerateUID(const std::string bufferUID)
	{
		return bufferUID;
	}

protected:
	UINT32 m_stride;
	std::string m_bufferUID;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

