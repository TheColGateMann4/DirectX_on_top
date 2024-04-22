#pragma once
#include "Camera.h"
#include "DepthTextureCube.h"
#include "CubeTextureDrawingOrder.h"

class ShadowCamera : public Camera
{
public:
	ShadowCamera(GFX& gfx, float aspectRatio = 0.0f);

public:
	virtual void RenderThisObjectOnScene() const noexcept(!IS_DEBUG) override;

	void SetCurrentCubeDrawingIndex(GFX& gfx, CubeTextureDrawingOrder index, RenderTarget* renderTarget);

	std::shared_ptr<DepthTextureCube>& GetDepthTextureCube();

private:
	std::shared_ptr<DepthTextureCube> m_depthTextureCube;
};

