#pragma once
#include "Includes.h"
#include "Bindable.h"


class Texture : public Bindable
{
public:
	Texture(GFX& gfx, const std::wstring imagePath);
	void Bind(GFX& gfx) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11Resource> pTexture;
};

