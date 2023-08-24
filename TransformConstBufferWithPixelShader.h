#pragma once
#include "TransformConstBuffer.h"
#include "ConstantBuffers.h"

class TransformConstBufferWithPixelShader : public TransformConstBuffer
{
public:
	TransformConstBufferWithPixelShader(GFX& gfx, const Shape& parent, UINT32 vertexBufferSlot, UINT32 pixelBufferSlot)
		:
		TransformConstBuffer(gfx, parent, vertexBufferSlot),
		pcbuf(gfx, pixelBufferSlot)
	{}

public:
	void Bind(GFX& gfx) noexcept override
	{
		const auto constBuffer = GetBuffer(gfx);
		TransformConstBuffer::UpdateAndBindConstBuffer(gfx, constBuffer);
		UpdateAndBindConstBuffer(gfx, constBuffer);
	}

private:
	void UpdateAndBindConstBuffer(GFX& gfx, const VertexConstBufferIn& constBuffer) noexcept
	{
		pcbuf.Update(gfx, constBuffer);
		pcbuf.Bind(gfx);
	}

protected:
	PixelConstantBuffer<TransformConstBuffer::VertexConstBufferIn> pcbuf;

};

