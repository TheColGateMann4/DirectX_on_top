#include "IndexBuffer.h"
#include "ErrorMacros.h"

IndexBuffer::IndexBuffer(GFX& gfx, const std::string bufferUID, const std::vector<UINT32>& indices)
	: m_count( (UINT32)indices.size()),
	m_bufferUID(bufferUID)
{
	HRESULT hr;

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = (UINT32)(m_count * sizeof(UINT32));
	ibd.StructureByteStride = sizeof(UINT32);

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();

	THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &pIndexBuffer));
}

VOID IndexBuffer::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
}

UINT32 IndexBuffer::GetCount() const noexcept
{
	return m_count;
}