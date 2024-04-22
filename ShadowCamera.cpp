#include "ShadowCamera.h"


ShadowCamera::ShadowCamera(GFX& gfx, float aspectRatio)
	:
	Camera(gfx, {0.0f, 0.0f, 0.0f}, aspectRatio)
{
	m_depthTextureCube = std::make_shared<DepthTextureCube>(gfx, 3);

	SetVisibilityInHierarchy(false);
}


void ShadowCamera::RenderThisObjectOnScene() const noexcept(!IS_DEBUG)
{
	// we are not drawing any indicators since this object is meant to be invisible
}

void ShadowCamera::SetCurrentCubeDrawingIndex(GFX& gfx, CubeTextureDrawingOrder index, RenderTarget* renderTarget)
{
	m_depthTextureCube->BindDepthTextureCubeSide(gfx, index, renderTarget);
}

std::shared_ptr<DepthTextureCube>& ShadowCamera::GetDepthTextureCube()
{
	return m_depthTextureCube;
}