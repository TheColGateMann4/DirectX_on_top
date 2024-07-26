#pragma once
#include "RenderJobPass.h"
#include "RasterizerState.h"
#include "ConstantBuffers.h"

class NormalRenderPass : public RenderJobPass
{
public:
	NormalRenderPass(class GFX& gfx, const char* name);

public:
	virtual void Render(GFX& gfx) const noexcept(!IS_DEBUG) override;

	void ShowWindow(GFX& gfx, bool show);

private:
	bool shadowTextureMultiSampling = true;
};

