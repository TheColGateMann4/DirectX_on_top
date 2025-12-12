#pragma once
#include "RenderJobPass.h"
#include "Bindable/ConstantBuffers.h"

class OutlineRenderPass : public RenderJobPass
{
public:
	OutlineRenderPass(class GFX& gfx, const char* name, const int width, const int height);

	virtual void Render(GFX& gfx) const noexcept(!IS_DEBUG) override;
};

