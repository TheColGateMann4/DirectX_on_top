#pragma once
#include "ErrorMacros.h"
#include "includes.h"
#include "Graphics.h"

class Bindable
{
public:
	virtual void Bind(GFX& gfx) noexcept = 0;
	virtual ~Bindable() = default;

	virtual std::string GetUID() const noexcept	{ return ""; };

protected:
	static ID3D11DeviceContext* GetDeviceContext(GFX& gfx) noexcept;
	static ID3D11Device* GetDevice(GFX& gfx) noexcept;
};

