#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"
#include <memory.h>

class TransformConstBuffer : public Bindable
{
public:
	TransformConstBuffer(GFX& gfx, const Shape& parent, UINT32 slot = 0)
		: 
		m_parent(parent),
		vcbuf(gfx, slot)
	{}

protected:
	struct VertexConstBufferIn {
		DirectX::XMMATRIX model, modelViewProjection;
	};

	void Bind(GFX& gfx) noexcept override
	{
		const auto constBuffer = GetBuffer(gfx);
		UpdateAndBindConstBuffer(gfx, constBuffer);
	}

protected:
	void UpdateAndBindConstBuffer(GFX& gfx, const VertexConstBufferIn& constBuffer) noexcept
	{
		vcbuf.Update(gfx, constBuffer);
		vcbuf.Bind(gfx);
	}

	VertexConstBufferIn GetBuffer(GFX& gfx) noexcept
	{
		const auto ModelTransformView = m_parent.GetTranformMatrix() * gfx.camera.GetCamera();
		return { DirectX::XMMatrixTranspose(ModelTransformView),
			DirectX::XMMatrixTranspose(
				ModelTransformView *
				gfx.camera.GetProjection()
		)};
	}

protected:
	VertexConstantBuffer<VertexConstBufferIn> vcbuf;
	
	const Shape& m_parent;
};