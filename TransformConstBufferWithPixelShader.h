#pragma once
#include "Shape.h"
#include "TransformConstBuffer.h"
#include "ConstantBuffers.h"
#include "DynamicConstantBuffer.h"

class TransformConstBufferWithPixelShader : public TransformConstBuffer
{
public:
	TransformConstBufferWithPixelShader(GFX& gfx, const Shape& parent, UINT32 vertexBufferSlot, UINT32 pixelBufferSlot)
		:
		TransformConstBuffer(gfx, parent, vertexBufferSlot),
		pcbuf(gfx, TCBLayout, pixelBufferSlot, true)
	{}

public:
	void Bind(GFX& gfx) noexcept override
	{
		DynamicConstantBuffer::BufferData constBuffer(TCBLayout);

		GetBuffer(gfx, constBuffer);

		TransformConstBuffer::UpdateAndBindConstBuffer(gfx, constBuffer);
		UpdateAndBindConstBuffer(gfx, constBuffer);
	}

private:
	void UpdateAndBindConstBuffer(GFX& gfx, const DynamicConstantBuffer::BufferData& constBuffer) noexcept
	{
		pcbuf.Update(gfx, constBuffer);
		pcbuf.Bind(gfx);
	}
protected:
	DynamicConstantBuffer::BufferLayout TCBLayout{ "MAMA" };

	NonCachedBuffer pcbuf;

};

