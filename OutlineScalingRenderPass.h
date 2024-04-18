#pragma once
#include "FullscreenFilterRenderPass.h"
#include "Includes.h"
#include "ConstantBuffers.h"

class OutlineScalingRenderPass : public FullscreenFilterRenderPass
{
public:
	OutlineScalingRenderPass(class GFX& gfx, const char* name, const int width, const int height);

public:
	virtual void Render(GFX& gfx) const noexcept(!IS_DEBUG) override;

private:
	std::shared_ptr<CachedBuffer> outlineScalingBuffer;
};

