#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"
#include "DynamicConstantBuffer.h"

class GFX;

class TransformConstBuffer : public Bindable
{
public:
	TransformConstBuffer(GFX& gfx, const Shape& parent, UINT32 slot = 0, TargetShader targetShader = TargetVertexShader);

public:
	virtual void Initialize(GFX& gfx, UINT32 slot);

	virtual void Bind(GFX& gfx) noexcept override;

protected:
	void UpdateAndBindConstBuffer(GFX& gfx, const DynamicConstantBuffer::BufferData& constBuffer) noexcept;

	virtual void GetBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData) const noexcept;

protected:
	const Shape& m_parent;

	DynamicConstantBuffer::BufferLayout TCBLayout;

	std::shared_ptr<NonCachedBuffer> vcbuf;
	TargetShader m_targetShader;
};