#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"

class PSConstBuffer : public Bindable
{
public:
	PSConstBuffer(GFX& gfx)
		: vcbuf(gfx) {};
	VOID Bind(GFX& gfx, float time) noexcept
	{
		DirectX::XMFLOAT4 arr = {};
		arr.x = time;
		vcbuf.Update(gfx, arr);
		vcbuf.Bind(gfx);
	}
	VOID Bind(GFX& gfx) noexcept override {};

protected:
	PixelConstantBuffer<DirectX::XMFLOAT4> vcbuf;
};

