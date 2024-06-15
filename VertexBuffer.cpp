#include "VertexBuffer.h"
#include "Graphics.h"

VertexBuffer::VertexBuffer(GFX& gfx, const DynamicVertex::VertexBuffer& vertexBuffer)
	:
	VertexBuffer(gfx, "UNKNOWN", vertexBuffer)
{}

VertexBuffer::VertexBuffer(GFX& gfx, const std::string bufferUID, const DynamicVertex::VertexBuffer& vertexBuffer)
	:
	m_stride((UINT32)vertexBuffer.GetLayout().GetByteSize()),
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
	vertexBufferResourceData.pSysMem = vertexBuffer.GetConstData();

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&vertexBufferDesc, &vertexBufferResourceData, &pVertexBuffer));
}

VOID VertexBuffer::Bind(GFX& gfx) noexcept
{
	const UINT offset = NULL;
	GFX::GetDeviceContext(gfx)->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &m_stride, &offset);
}