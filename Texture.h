#pragma once
#include "Includes.h"
#include "Bindable.h"


class Texture : public Bindable
{
public:
	Texture(GFX& gfx, const std::wstring imagePath, UINT32 slot = 0);
	void Bind(GFX& gfx) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	UINT32 m_slot;
};

