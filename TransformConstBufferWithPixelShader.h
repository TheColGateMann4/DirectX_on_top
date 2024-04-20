#pragma once
#include "TransformConstBuffer.h"
#include "ConstantBuffers.h"

class TransformConstBufferWithPixelShader : public TransformConstBuffer
{
public:
	TransformConstBufferWithPixelShader(GFX& gfx, const class Shape& parent, UINT32 vertexBufferSlot, UINT32 pixelBufferSlot);

public:
	void Bind(GFX& gfx) noexcept override;

private:
	void UpdateAndBindConstBuffer(GFX& gfx, const class DynamicConstantBuffer::BufferData& constBuffer) noexcept;
protected:
	std::shared_ptr<NonCachedBuffer> pcbuf;
};

