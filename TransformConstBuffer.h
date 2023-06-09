#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"

class TransformConstBuffer : public Bindable
{
public:
	TransformConstBuffer(GFX& gfx, const Shape& parent);
	VOID Bind(GFX& gfx, float currtime) noexcept;
	VOID Bind(GFX& gfx) noexcept override;

protected:
	struct waveData { DirectX::XMMATRIX matrix; float currtime; };
	VertexConstantBuffer<waveData> vcbuf;
	const Shape& m_parent;
};