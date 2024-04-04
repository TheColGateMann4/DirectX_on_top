#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include "wrl_no_warnings.h"

class SamplerState : public Bindable
{
public:
	SamplerState(GFX& gfx, bool anisotropicFiltering, bool mirror = false);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<SamplerState> GetBindable(GFX& gfx, bool anisotropicFiltering, bool mirror)
	{
		return BindableList::GetBindable<SamplerState>(gfx, anisotropicFiltering, mirror);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_anisotropicFiltering, m_mirror);
	};

	static std::string GetStaticUID(bool anisotropicFiltering, bool mirror) noexcept
	{
		return GenerateUID(anisotropicFiltering, mirror);
	};

private:
	static std::string GenerateUID(bool anisotropicFiltering, bool mirror)
	{
		return std::string{} + char(anisotropicFiltering + '0') + char(mirror + '0');
	}

protected:
	bool m_anisotropicFiltering;
	bool m_mirror;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
};

