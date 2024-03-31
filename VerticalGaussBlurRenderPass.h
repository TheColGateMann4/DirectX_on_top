#pragma once
#include "FullscreenFilterRenderPass.h"
#include "ConstantBuffers.h"
#include "Includes.h"

class VerticalGaussBlurRenderPass : public FullscreenFilterRenderPass
{
public:
	VerticalGaussBlurRenderPass(class GFX& gfx, const char* name);

public:
	virtual void Render(GFX& gfx) const noexcept(!_DEBUG) override;

private:
	std::shared_ptr<CachedBuffer> gaussBlurDirectionData;
	std::shared_ptr<CachedBuffer> gaussBlurCooficientsData;
};

