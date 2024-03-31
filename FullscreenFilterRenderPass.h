#pragma once
#include "RenderJobPass.h"

class FullscreenFilterRenderPass : public RenderJobPass
{
public:
	FullscreenFilterRenderPass(class GFX& gfx, const char* name);

public:
	virtual void Render(GFX& gfx) const noexcept(!_DEBUG) override;
};

