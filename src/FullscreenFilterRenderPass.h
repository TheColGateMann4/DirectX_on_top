#pragma once
#include "RenderBindablePass.h"

class FullscreenFilterRenderPass : public RenderBindablePass
{
public:
	FullscreenFilterRenderPass(class GFX& gfx, const char* name);

public:
	virtual void Render(GFX& gfx) const noexcept(!IS_DEBUG) override;
};

