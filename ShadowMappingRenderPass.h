#pragma once
#include "RenderJobPass.h"
#include "DepthTextureCube.h"

class Scene;
class ShadowCamera;

class ShadowMappingRenderPass : public RenderJobPass
{
public:
	ShadowMappingRenderPass(class GFX& gfx, const char* name);

public:
	virtual void Render(GFX& gfx) const noexcept(!IS_DEBUG) override;

private:
	void RenderFromAllAngles(GFX& gfx, ShadowCamera* shadowCamera) const noexcept(!IS_DEBUG);

	void RenderModels(GFX& gfx) const noexcept(!IS_DEBUG);

private:
	std::shared_ptr<CachedBuffer> shadowCameraTransformBuffer;
	mutable std::shared_ptr<DepthTextureCube> depthTextureCube;
};