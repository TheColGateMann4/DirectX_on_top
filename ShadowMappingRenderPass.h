#pragma once
#include "RenderJobPass.h"

class Scene;

class ShadowMappingRenderPass : public RenderJobPass
{
public:
	ShadowMappingRenderPass(class GFX& gfx, const char* name, Scene& scene);

public:
	virtual void Render(GFX& gfx) const noexcept(!IS_DEBUG) override;

private:
	std::shared_ptr<CachedBuffer> shadowCameraTransformBuffer;
	Scene* m_scene;
};