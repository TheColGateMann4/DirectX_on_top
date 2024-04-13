#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include "wrl_no_warnings.h"

class SamplerState : public Bindable
{
public:
	enum Mode
	{
		WRAP = 1,
		MIRROR = 2,
		CLAMP = 3,
		BORDER = 4,
		MIRROR_ONCE = 5
	};

public:
	SamplerState(GFX& gfx, bool anisotropicFiltering, Mode samplerMode, size_t slot = 0);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<SamplerState> GetBindable(GFX& gfx, bool anisotropicFiltering, Mode samplerMode, size_t slot = 0)
	{
		return BindableList::GetBindable<SamplerState>(gfx, anisotropicFiltering, samplerMode, slot);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_anisotropicFiltering, m_samplerMode, m_slot);
	};

	static std::string GetStaticUID(bool anisotropicFiltering, Mode samplerMode, size_t slot) noexcept
	{
		return GenerateUID(anisotropicFiltering, samplerMode, slot);
	};

private:
	static std::string GenerateUID(bool anisotropicFiltering, Mode samplerMode, size_t slot)
	{
		return std::to_string(anisotropicFiltering) + '@' + std::to_string(samplerMode) + '@' + std::to_string(slot);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
	bool m_anisotropicFiltering;
	Mode m_samplerMode;
	size_t m_slot;
};

