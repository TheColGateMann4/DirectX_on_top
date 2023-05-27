#pragma once
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(GFX& gfx, const std::wstring& path);
	VOID Bind(GFX& gfx) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};