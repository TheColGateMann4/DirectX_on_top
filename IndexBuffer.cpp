#include "IndexBuffer.h"
#include "ErrorMacros.h"
#include "Graphics.h"

IndexBuffer::IndexBuffer(GFX& gfx, const std::vector<UINT32>& indices)
	: 
	IndexBuffer(gfx, "UNKNOWN", indices) 
{

}

IndexBuffer::IndexBuffer(GFX& gfx, const std::string bufferUID, const std::vector<UINT32>& indices)
	: 
	m_bufferUID(bufferUID),
	m_count(indices.size())
{
	HRESULT hr;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = m_count * sizeof(UINT32);
	bufferDesc.StructureByteStride = sizeof(UINT32);

	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = indices.data();

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subresourceData, &pIndexBuffer));
}

void IndexBuffer::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
}

UINT32 IndexBuffer::GetCount() const noexcept
{
	return m_count;
}