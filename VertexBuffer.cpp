#include "VertexBuffer.h"

VOID VertexBuffer::Bind(GFX& gfx) noexcept
{
	const UINT offset = NULL;
	VertexBuffer::GetDeviceContext(gfx)->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &(VertexBuffer::stride), &offset);
}