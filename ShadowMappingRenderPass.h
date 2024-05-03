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

	void UpdateCameraData(GFX& gfx, ShadowCamera* shadowCamera) const;

	void UpdateCameraTransformBuffer(GFX& gfx, ShadowCamera* shadowCamera) const;

private:
	std::shared_ptr<CachedBuffer> shadowCameraData;
	std::shared_ptr<CachedBuffer> shadowCameraTransformBuffer;
	std::shared_ptr<DepthTextureCube> depthTextureCube;

	mutable bool cameraDataInitialized = false;
	mutable float prevNearZ = 0.0f;
	mutable float prevFarZ = 0.0f;
};