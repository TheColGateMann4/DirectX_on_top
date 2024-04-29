#include "ShadowCamera.h"


ShadowCamera::ShadowCamera(GFX& gfx)
	:
	Camera(gfx, {0.0f, 0.0f, 0.0f}, 1.0f)
{
	SetVisibilityInHierarchy(false);
}


void ShadowCamera::RenderThisObjectOnScene() const noexcept(!IS_DEBUG)
{
	// we are not drawing any indicators since this object is meant to be invisible
}