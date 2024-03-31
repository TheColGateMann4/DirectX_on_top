#pragma once
#include "RenderJobPass.h"

class BlurOutlineRenderPass : public RenderJobPass
{
public:
	BlurOutlineRenderPass(class GFX& gfx, const char* name, float resolutionX, float resolutionY);

public:
	virtual void Render(GFX& gfx) const noexcept(!_DEBUG) override;
};

