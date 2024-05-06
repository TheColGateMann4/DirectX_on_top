#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include "wrl_no_warnings.h"

class SamplerState : public Bindable
{
public:
	enum Mode : size_t
	{
		WRAP = D3D11_TEXTURE_ADDRESS_WRAP,
		MIRROR = D3D11_TEXTURE_ADDRESS_MIRROR,
		CLAMP = D3D11_TEXTURE_ADDRESS_CLAMP,
		BORDER = D3D11_TEXTURE_ADDRESS_BORDER,
		MIRROR_ONCE = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE
	};
	enum Comparison : size_t
	{
		NEVER = D3D11_COMPARISON_NEVER,
		LESS = D3D11_COMPARISON_LESS,
		EQUAL = D3D11_COMPARISON_EQUAL,
		LESS_EQUAL = D3D11_COMPARISON_LESS_EQUAL,
		GREATER = D3D11_COMPARISON_GREATER,
		NOT_EQUAL = D3D11_COMPARISON_NOT_EQUAL,
		GREATER_EQUAL = D3D11_COMPARISON_GREATER_EQUAL,
		ALWAYS = D3D11_COMPARISON_ALWAYS
	};
	enum Filter : size_t
	{
		POINT = D3D11_FILTER_MIN_MAG_MIP_POINT,
		BILINEAR = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		ANISOTROPIC = D3D11_FILTER_ANISOTROPIC
	};

public:
	SamplerState(GFX& gfx, Mode samplerMode, size_t slot = 0, Comparison comparison = NEVER, Filter filter = ANISOTROPIC, bool isPixelShaderResource = true);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<SamplerState> GetBindable(GFX& gfx, Mode samplerMode, size_t slot = 0, Comparison comparison = NEVER, Filter filter = ANISOTROPIC, bool isPixelShaderResource = true)
	{
		return BindableList::GetBindable<SamplerState>(gfx, samplerMode, slot, comparison, filter, isPixelShaderResource);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_samplerMode, m_slot, m_comparison, m_filter, m_isPixelShaderResource);
	};

	static std::string GetStaticUID(Mode samplerMode, size_t slot, Comparison comparison, Filter filter, bool isPixelShaderResource) noexcept
	{
		return GenerateUID(samplerMode, slot, comparison, filter, isPixelShaderResource);
	};

private:
	static std::string GenerateUID(Mode samplerMode, size_t slot, Comparison comparison, Filter filter, bool isPixelShaderResource)
	{
		return std::to_string(samplerMode) + '@' + std::to_string(slot) + '@' + std::to_string(comparison) + '@' + std::to_string(filter) + '@' + std::to_string(isPixelShaderResource);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
	Mode m_samplerMode;
	size_t m_slot;
	Comparison m_comparison;
	Filter m_filter;
	bool m_isPixelShaderResource;
};
