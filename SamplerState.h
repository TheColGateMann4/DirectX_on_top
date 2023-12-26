#pragma once
#include "Bindable.h"
#include "BindableList.h"

class SamplerState : public Bindable
{
public:
	SamplerState(GFX& gfx, D3D11_TEXTURE_ADDRESS_MODE mode);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<SamplerState> GetBindable(GFX& gfx, D3D11_TEXTURE_ADDRESS_MODE mode)
	{
		return BindableList::GetBindable<SamplerState>(gfx, mode);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_mode);
	};

	static std::string GetStaticUID(D3D11_TEXTURE_ADDRESS_MODE mode) noexcept
	{
		return GenerateUID(mode);
	};

private:
	static std::string GenerateUID(D3D11_TEXTURE_ADDRESS_MODE mode)
	{
		return std::to_string(mode);
	}

protected:
	D3D11_TEXTURE_ADDRESS_MODE m_mode;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
};

