#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"

class TransformConstBuffer : public Bindable
{
public:
	TransformConstBuffer(GFX& gfx, const Shape& parent);
	VOID Bind(GFX& gfx) noexcept override;

protected:
	VertexConstantBuffer<DirectX::XMMATRIX> vcbuf;
	const Shape& m_parent;
};