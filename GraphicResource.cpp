#include "GraphicResource.h"
#include "Graphics.h"

ID3D11DeviceContext* GraphicResource::GetDeviceContext(GFX& gfx) noexcept
{
	return gfx.pDeviceContext.Get();
}

ID3D11Device* GraphicResource::GetDevice(GFX& gfx) noexcept
{
	return gfx.pDevice.Get();
}