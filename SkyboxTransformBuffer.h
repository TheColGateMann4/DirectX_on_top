#pragma once
#include "TransformConstBuffer.h"

class GFX;

class SkyboxTransformBuffer : public TransformConstBuffer
{
public:
	SkyboxTransformBuffer(GFX& gfx, const class Shape& parent, UINT32 slot = 0);

public:
	virtual void Initialize(GFX& gfx, UINT32 slot) override;

	virtual void GetBuffer(GFX& gfx, class DynamicConstantBuffer::BufferData& bufferData) const noexcept override;
};