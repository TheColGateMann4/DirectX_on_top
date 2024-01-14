#pragma once
#include "Includes.h"

class GraphicResource
{
protected:
	static ID3D11DeviceContext* GetDeviceContext(class GFX& gfx) noexcept;
	static ID3D11Device* GetDevice(class GFX& gfx) noexcept;
};