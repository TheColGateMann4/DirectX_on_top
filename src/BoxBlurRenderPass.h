#pragma once
#include "FullscreenFilterRenderPass.h"

class BoxBlurRenderPass : public FullscreenFilterRenderPass
{
public:
	BoxBlurRenderPass(class GFX& gfx, const char* name);

private:
	std::shared_ptr<CachedBuffer> blurData;
	std::shared_ptr<RenderTargetWithTexture> pixelShaderTexture;
};
