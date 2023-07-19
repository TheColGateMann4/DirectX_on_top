#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"

class TransformConstBuffer : public Bindable
{
public:
	TransformConstBuffer(GFX& gfx, const Shape& parent, UINT32 slot = 0)
		: m_parent(parent), vcbuf(gfx, slot) {}

	void Bind(GFX& gfx) noexcept override
	{
		const DirectX::XMMATRIX ModelTransformView = m_parent.GetTranformMatrix() * gfx.camera.GetCamera();
		vcbuf.Update(gfx,
			{
				DirectX::XMMatrixTranspose(ModelTransformView),
				DirectX::XMMatrixTranspose(
				ModelTransformView *
				gfx.camera.GetProjection()
				)
			});

		vcbuf.Bind(gfx);
	}

protected:
	struct VertexConstBufferIn {
		DirectX::XMMATRIX model, modelViewProjection;
	};
	VertexConstantBuffer<VertexConstBufferIn> vcbuf;
	const Shape& m_parent;
};