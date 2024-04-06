#include "VertexBuffer.h"
#include "Graphics.h"

VOID VertexBuffer::Bind(GFX& gfx) noexcept
{
	const UINT offset = NULL;
	GFX::GetDeviceContext(gfx)->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &m_stride, &offset);
}