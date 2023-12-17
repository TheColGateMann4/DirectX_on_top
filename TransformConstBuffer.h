#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"
#include "DynamicConstantBuffer.h"
#include <memory.h>

class TransformConstBuffer : public Bindable
{
public:
	TransformConstBuffer(GFX& gfx, const Shape& parent, UINT32 slot = 0)
		: 
		m_parent(parent),
		vcbuf(gfx, TCBLayout, slot, false)
	{}

protected:

	void Bind(GFX& gfx) noexcept override
	{
		DynamicConstantBuffer::BufferData constBuffer(TCBLayout);

		GetBuffer(gfx, constBuffer);

		UpdateAndBindConstBuffer(gfx, constBuffer);
	}

protected:
	void UpdateAndBindConstBuffer(GFX& gfx, const DynamicConstantBuffer::BufferData& constBuffer) noexcept
	{
		vcbuf.Update(gfx, constBuffer);
		vcbuf.Bind(gfx);
	}

	void GetBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData) noexcept
	{
		const auto ModelTransformView = m_parent.GetTranformMatrix() * gfx.camera.GetCamera();

		bufferData += DirectX::XMMatrixTranspose(ModelTransformView);
		bufferData += DirectX::XMMatrixTranspose(ModelTransformView * gfx.camera.GetProjection());
	}

protected:
	const Shape& m_parent;

	DynamicConstantBuffer::BufferLayout TCBLayout{"MAMA"};

	NonCachedBuffer vcbuf;
};