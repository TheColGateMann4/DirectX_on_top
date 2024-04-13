#pragma once
#include "RenderJobPass.h"

class NormalRenderPass : public RenderJobPass
{
public:
	NormalRenderPass(class GFX& gfx, const char* name);

public:
	virtual void Render(GFX& gfx) const noexcept(!_DEBUG) override;
};

