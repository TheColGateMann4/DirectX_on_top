#pragma once
#include "FullscreenFilterRenderPass.h"
#include "ConstantBuffers.h"
#include "Includes.h"

class HorizontalGaussBlurRenderPass : public FullscreenFilterRenderPass
{
public:
	HorizontalGaussBlurRenderPass(class GFX& gfx, const char* name, const int width, const int height);

public:
	virtual void Render(GFX& gfx) const noexcept(!_DEBUG) override;

private:
	std::shared_ptr<CachedBuffer> gaussBlurDirectionData;
};