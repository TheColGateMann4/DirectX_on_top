#pragma once
#include "Camera.h"
#include "DepthTextureCube.h"
#include "CubeTextureDrawingOrder.h"

class ShadowCamera : public Camera
{
public:
	ShadowCamera(GFX& gfx);

public:
	virtual void RenderThisObjectOnScene() const noexcept(!IS_DEBUG) override;
};

