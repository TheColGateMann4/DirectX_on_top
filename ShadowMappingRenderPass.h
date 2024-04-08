#pragma once
#include "RenderJobPass.h"

class ShadowMappingRenderPass : public RenderJobPass
{
public:
	ShadowMappingRenderPass(class GFX& gfx, const char* name);

public:
	virtual void Render(GFX& gfx) const noexcept(!_DEBUG) override;
};