#pragma once
#include "Shape.h"
#include "ConstantBuffers.h"

class PSConstBuffer : public Bindable
{
public:
	PSConstBuffer(GFX& gfx, UINT32 slot = 0)
		: vcbuf(gfx, slot) {};
	VOID Bind(GFX& gfx, float time) noexcept
	{
		DirectX::XMFLOAT3 arr = {};
		arr.x = time;
		vcbuf.Update(gfx, arr);
		vcbuf.Bind(gfx);
	}
	VOID Bind(GFX& gfx) noexcept override {};

protected:
	PixelConstantBuffer<DirectX::XMFLOAT3> vcbuf;
};

