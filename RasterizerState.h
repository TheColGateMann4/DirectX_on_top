#pragma once
#include "Includes.h"
#include "Graphics.h"
#include "Bindable.h"
#include "BindableList.h"

class RasterizerState : public Bindable
{
public:
	RasterizerState(GFX& gfx, bool disableBackfaceCulling, INT depthBias = D3D11_DEFAULT_DEPTH_BIAS, FLOAT depthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP, FLOAT slopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS);

public:
	void ChangeDepthValues(GFX& gfx, INT depthBias = D3D11_DEFAULT_DEPTH_BIAS, FLOAT depthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP, FLOAT slopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS);

	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<RasterizerState> GetBindable(GFX& gfx, bool disableBackfaceCulling, INT depthBias = D3D11_DEFAULT_DEPTH_BIAS, FLOAT depthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP, FLOAT slopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS)
	{
		return BindableList::GetBindable<RasterizerState>(gfx, disableBackfaceCulling, depthBias, depthBiasClamp, slopeScaledDepthBias);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_disableBackfaceCulling, m_depthBias, m_depthBiasClamp, m_slopeScaledDepthBias);
	};

	static std::string GetStaticUID(bool disableBackfaceCulling, INT depthBias, FLOAT depthBiasClamp, FLOAT slopeScaledDepthBias) noexcept
	{
		return GenerateUID(disableBackfaceCulling, depthBias, depthBiasClamp, slopeScaledDepthBias);
	};

private:
	static std::string GenerateUID(bool disableBackfaceCulling, INT depthBias, FLOAT depthBiasClamp, FLOAT slopeScaledDepthBias)
	{
		return std::to_string(disableBackfaceCulling) + '@' + std::to_string(depthBias) + '@' + std::to_string(depthBiasClamp) + '@' + std::to_string(slopeScaledDepthBias);
	}

protected:
	bool m_disableBackfaceCulling;
	INT m_depthBias;
	FLOAT m_depthBiasClamp;
	FLOAT m_slopeScaledDepthBias;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pResterizerState;
};

