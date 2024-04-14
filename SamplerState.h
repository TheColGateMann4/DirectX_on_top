#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include "wrl_no_warnings.h"

class SamplerState : public Bindable
{
public:
	enum Mode : size_t
	{
		WRAP = 1,
		MIRROR = 2,
		CLAMP = 3,
		BORDER = 4,
		MIRROR_ONCE = 5
	};
	enum Comparison : size_t
	{
		NEVER = 1,
		LESS = 2,
		EQUAL = 3,
		LESS_EQUAL = 4,
		GREATER = 5,
		NOT_EQUAL = 6,
		GREATER_EQUAL = 7,
		ALWAYS = 8
	};
	enum Filter : size_t
	{
		POINT = 0x0,
		BILINEAR = 0x1,
		ANISOTROPIC = 0x55,
	};

public:
	SamplerState(GFX& gfx, Mode samplerMode, size_t slot = 0, Comparison comparison = NEVER, Filter filter = ANISOTROPIC);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<SamplerState> GetBindable(GFX& gfx, Mode samplerMode, size_t slot = 0, Comparison comparison = NEVER, Filter filter = ANISOTROPIC)
	{
		return BindableList::GetBindable<SamplerState>(gfx, samplerMode, slot, comparison, filter);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_samplerMode, m_slot, m_comparison, m_filter);
	};

	static std::string GetStaticUID(Mode samplerMode, size_t slot, Comparison comparison, Filter filter) noexcept
	{
		return GenerateUID(samplerMode, slot, comparison, filter);
	};

private:
	static std::string GenerateUID(Mode samplerMode, size_t slot, Comparison comparison, Filter filter)
	{
		return std::to_string(samplerMode) + '@' + std::to_string(slot) + '@' + std::to_string(comparison) + '@' + std::to_string(filter);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
	Mode m_samplerMode;
	size_t m_slot;
	Comparison m_comparison;
	Filter m_filter;
};
