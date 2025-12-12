#pragma once
#include "RenderJobPass.h"
#include "DepthTextureCube.h"
#include "Bindable/RasterizerState.h"
#include "RenderFirstCallPass.h"

class Scene;
class ShadowCamera;

class ShadowMappingRenderPass : public RenderFirstCallJobPass
{
public:
	ShadowMappingRenderPass(class GFX& gfx, const char* name);

protected:
	virtual void FirstRenderCall(GFX& gfx) const noexcept(!IS_DEBUG) override;

	virtual void FurtherRenderCall(GFX& gfx) const noexcept(!IS_DEBUG) override;

public:
	void ShowWindow(GFX& gfx, bool show);

private:
	void RenderFromAllAngles(GFX& gfx, ShadowCamera* shadowCamera) const noexcept(!IS_DEBUG);

	void RenderModels(GFX& gfx, std::shared_ptr<DepthStencilView> sideDepthStencilView) const noexcept(!IS_DEBUG);

	void UpdateCameraData(GFX& gfx, ShadowCamera* shadowCamera) const;

	void UpdateCameraTransformBuffer(GFX& gfx, ShadowCamera* shadowCamera) const;

private:
	std::shared_ptr<CachedBuffer> shadowCameraData;
	std::shared_ptr<CachedBuffer> shadowCameraTransformBuffer;
	std::shared_ptr<DepthTextureCube> depthTextureCube;

	mutable bool cameraDataInitialized = false;
	mutable float prevNearZ = 0.0f;
	mutable float prevFarZ = 0.0f;
	mutable int prevPCF = 0;

	std::shared_ptr<RasterizerState> shadowRasterizer;

	INT bias = 40;
	FLOAT biasClamp = 0.00365f;
	FLOAT slopeScaledDepthBias = 3.464f;
	INT pcf = 4;
	mutable bool m_captureFrameAngles = false;
};