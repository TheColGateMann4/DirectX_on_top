#pragma once
#include "TransformConstBuffer.h"

class GFX;

class SkyboxTransformBuffer : public TransformConstBuffer
{
public:
	SkyboxTransformBuffer(GFX& gfx, const Shape& parent, std::vector<TargetShaderBufferBinding> targetBuffers);

	static std::shared_ptr<SkyboxTransformBuffer> GetBindable(GFX& gfx, const Shape& parent, std::vector<TargetShaderBufferBinding> targetBuffers)
	{
		return std::make_unique<SkyboxTransformBuffer>(gfx, parent, targetBuffers);
	}

protected:
	virtual void GetBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData) const noexcept override;

	virtual DynamicConstantBuffer::BufferLayout GetBufferLayout() const noexcept override;
};