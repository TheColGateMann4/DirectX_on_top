#pragma once
#include "Bindable.h"

class SamplerState : public Bindable
{
public:
	SamplerState(GFX& gfx, D3D11_TEXTURE_ADDRESS_MODE mode);
	void Bind(GFX& gfx) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
};

