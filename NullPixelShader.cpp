#include "NullPixelShader.h"
#include "Graphics.h"

VOID NullPixelShader::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->PSSetShader(nullptr, nullptr, NULL);
}