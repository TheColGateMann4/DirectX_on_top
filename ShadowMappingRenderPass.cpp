#include "ShadowMappingRenderPass.h"
#include "RenderPassInput.h"
#include "RenderPassOuput.h"
#include "BindableClassesMacro.h"
#include "Scene.h"
#include "Pointlight.h"

ShadowMappingRenderPass::ShadowMappingRenderPass(GFX& gfx, const char* name, class Scene& scene)
	:
	RenderJobPass(name),
	m_scene(&scene)
{
	m_depthStencilView = std::make_shared<DepthStencilViewWithTexture>(gfx, DepthStencilView::Mode::DepthOnly);
	m_renderTarget = std::make_shared<RenderTarget>(gfx, gfx.GetWidth(), gfx.GetHeight());

	RegisterOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("shadowMap", &m_depthStencilView));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::Off));
	AddBindable(SamplerState::GetBindable(gfx, false, false));
}

void ShadowMappingRenderPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	Camera* previousCamera = m_scene->GetCameraManager()->GetActiveCamera();
	Camera* shadowCamera = m_scene->GetLights().front()->GetShadowCamera();

	m_scene->GetCameraManager()->SetActiveCameraByPtr(shadowCamera);

	{
		m_depthStencilView->Clear(gfx);
		m_renderTarget->Clear(gfx);

		RenderJobPass::Render(gfx);
	}

	m_scene->GetCameraManager()->SetActiveCameraByPtr(previousCamera);
}