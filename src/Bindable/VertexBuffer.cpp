#include "VertexBuffer.h"
#include "Graphics.h"

VertexBuffer::VertexBuffer(GFX& gfx, const DynamicVertex::VertexBuffer& vertexBuffer)
	:
	VertexBuffer(gfx, "UNKNOWN", vertexBuffer)
{}

VertexBuffer::VertexBuffer(GFX& gfx, const std::string bufferUID, const DynamicVertex::VertexBuffer& vertexBuffer)
	:
	m_stride((UINT32)vertexBuffer.GetLayout().GetByteSize()),
	m_bufferSize((UINT32)vertexBuffer.GetBytesSize()),
	m_bufferUID(bufferUID),
	m_vertexLayout(vertexBuffer.GetLayout())
{
	HRESULT hr;

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.CPUAccessFlags = NULL;
	vertexBufferDesc.MiscFlags = NULL;
	vertexBufferDesc.ByteWidth = m_bufferSize;
	vertexBufferDesc.StructureByteStride = m_stride;

	D3D11_SUBRESOURCE_DATA vertexBufferResourceData = {};
	vertexBufferResourceData.pSysMem = vertexBuffer.GetConstData();

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &pVertexBuffer));
}

void VertexBuffer::Bind(GFX& gfx) noexcept
{
	const UINT offset = NULL;
	GFX::GetDeviceContext(gfx)->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &m_stride, &offset);
}

ID3D11Buffer* VertexBuffer::Get() const
{
	return pVertexBuffer.Get();
}

UINT32 VertexBuffer::GetBufferByteSize() const
{
	return m_bufferSize;
}

UINT32 VertexBuffer::GetBufferSize() const
{
	return m_bufferSize / m_stride;
}

const DynamicVertex::VertexLayout& VertexBuffer::GetLayout() const
{
	return m_vertexLayout;
}