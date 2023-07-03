#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"

class TransformConstBuffer : public Bindable
{
public:
	TransformConstBuffer(GFX& gfx, const Shape& parent)
		: m_parent(parent), vcbuf(gfx) {}

	VOID Bind(GFX& gfx) noexcept override
	{
		vcbuf.Update(gfx, 
			DirectX::XMMatrixTranspose(
				m_parent.GetTranformMatrix() * 
				gfx.camera.GetCamera() *
				gfx.camera.GetProjection()
		));

		vcbuf.Bind(gfx);
	}

protected:
	VertexConstantBuffer<DirectX::XMMATRIX> vcbuf;
	const Shape& m_parent;
};