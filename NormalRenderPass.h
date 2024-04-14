#pragma once
#include "RenderJobPass.h"
#include "RasterizerState.h"
#include "ConstantBuffers.h"
#include "ShadowSamplerStateManager.h"

class NormalRenderPass : public RenderJobPass
{
public:
	NormalRenderPass(class GFX& gfx, const char* name);

public:
	virtual void Render(GFX& gfx) const noexcept(!_DEBUG) override;

	void ShowWindow(GFX& gfx);

private:
	std::shared_ptr<RasterizerState> shadowRasterizerNoBackculling;
	std::shared_ptr<RasterizerState> shadowRasterizerWithculling;

	std::shared_ptr<CachedBuffer> shadowSettings;

	INT bias = D3D11_DEFAULT_DEPTH_BIAS;
	FLOAT biasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	FLOAT slopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;

	ShadowSamplerStateManager samplerStateManager;
};

