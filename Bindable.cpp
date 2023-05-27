#include "Bindable.h"


ID3D11DeviceContext* Bindable::GetDeviceContext(GFX& gfx) noexcept
{
	return gfx.pDeviceContext.Get();
}

ID3D11Device* Bindable::GetDevice(GFX& gfx) noexcept
{
	return gfx.pDevice.Get();
}