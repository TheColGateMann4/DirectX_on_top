#include "TransformConstBufferWithPixelShader.h"
#include "DynamicConstantBuffer.h"

TransformConstBufferWithPixelShader::TransformConstBufferWithPixelShader(GFX& gfx, const Shape& parent, UINT32 vertexBufferSlot, UINT32 pixelBufferSlot)
	:
	TransformConstBuffer(gfx, parent, vertexBufferSlot)
{
	pcbuf = std::make_shared<NonCachedBuffer>(gfx, TCBLayout, pixelBufferSlot, TargetPixelShader);
}

void TransformConstBufferWithPixelShader::Bind(GFX& gfx) noexcept
{
	DynamicConstantBuffer::BufferData constBuffer(TCBLayout);

	GetBuffer(gfx, constBuffer);

	TransformConstBuffer::UpdateAndBindConstBuffer(gfx, constBuffer);
	UpdateAndBindConstBuffer(gfx, constBuffer);
}

void TransformConstBufferWithPixelShader::UpdateAndBindConstBuffer(GFX& gfx, const DynamicConstantBuffer::BufferData& constBuffer) noexcept
{
	pcbuf->Update(gfx, constBuffer);
	pcbuf->Bind(gfx);
}