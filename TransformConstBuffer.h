#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"
#include "DynamicConstantBuffer.h"
#include <memory.h>

class TransformConstBuffer : public Bindable
{
public:
	TransformConstBuffer(class GFX& gfx, const Shape& parent, UINT32 slot = 0);

public:
	virtual void Initialize(GFX& gfx, UINT32 slot);

	virtual void Bind(class GFX& gfx) noexcept override;

protected:
	void UpdateAndBindConstBuffer(class GFX& gfx, const DynamicConstantBuffer::BufferData& constBuffer) noexcept;

	virtual void GetBuffer(class GFX& gfx, DynamicConstantBuffer::BufferData& bufferData) const noexcept;

protected:
	const Shape& m_parent;

	DynamicConstantBuffer::BufferLayout TCBLayout;

	std::shared_ptr<NonCachedBuffer> vcbuf;
};