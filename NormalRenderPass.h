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
	virtual void Render(GFX& gfx) const noexcept(!IS_DEBUG) override;

private:
	std::shared_ptr<RasterizerState> shadowRasterizerNoBackculling;
	std::shared_ptr<RasterizerState> shadowRasterizerWithculling;

	std::shared_ptr<CachedBuffer> shadowSettings;

	INT bias = 40;
	FLOAT biasClamp = 0.00427f;
	FLOAT slopeScaledDepthBias = 0.58140f;

	ShadowSamplerStateManager samplerStateManager;
};

