#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"
#include "DynamicConstantBuffer.h"

class GFX;

class TransformConstBuffer : public Bindable
{
public:
	struct TargetShaderBufferBinding
	{
		TargetShader type = TargetVertexShader;
		UINT32 slot = 0;
	};

public:
	TransformConstBuffer(GFX& gfx, const Shape& parent, std::vector<TargetShaderBufferBinding> targetBuffers);

	// making this function because when we use regular std::make_unique we can't use initialization list,
	// we need to use vector and that would make our initialization syntax look confusing
	static std::shared_ptr<TransformConstBuffer> GetBindable(GFX& gfx, const Shape& parent, std::vector<TargetShaderBufferBinding> targetBuffers)
	{
		return std::make_unique<TransformConstBuffer>(gfx, parent, targetBuffers);
	}

	virtual void Bind(GFX& gfx) noexcept override;

protected:
	void UpdateAndBindConstBuffer(GFX& gfx, const DynamicConstantBuffer::BufferData& bufferData) noexcept;

	virtual void GetBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData) const noexcept;

	virtual DynamicConstantBuffer::BufferLayout GetBufferLayout() const noexcept;

protected:
	const Shape& m_parent;

	DynamicConstantBuffer::BufferLayout TCBLayout;
	std::vector<std::shared_ptr<NonCachedBuffer>> constBuffers;
	std::vector<TargetShaderBufferBinding> m_targetBuffers;
	bool initialized = false;
};