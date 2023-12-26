#include "NullPixelShader.h"

VOID NullPixelShader::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->PSSetShader(nullptr, nullptr, NULL);
}