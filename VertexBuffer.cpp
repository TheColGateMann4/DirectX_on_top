#include "VertexBuffer.h"

VOID VertexBuffer::Bind(GFX& gfx) noexcept
{
	const UINT offsets = NULL;
	VertexBuffer::GetDeviceContext(gfx)->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &(VertexBuffer:: stride), &offsets);
}