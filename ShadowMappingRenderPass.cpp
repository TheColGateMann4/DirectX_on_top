#include "ShadowMappingRenderPass.h"
#include "RenderPassInput.h"
#include "RenderPassOuput.h"
#include "BindableClassesMacro.h"

ShadowMappingRenderPass::ShadowMappingRenderPass(GFX& gfx, const char* name)
	:
	RenderJobPass(name)
{
	m_depthStencilView = std::make_shared<DepthStencilViewWithTexture>(gfx, DepthStencilView::Mode::DepthOnly);
	m_renderTarget = std::make_shared<RenderTarget>(gfx, gfx.GetWidth(), gfx.GetHeight());

	RegisterOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("shadowMap", &m_depthStencilView));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::Off));
	AddBindable(SamplerState::GetBindable(gfx, false, false));
}

void ShadowMappingRenderPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	m_depthStencilView->Clear(gfx);
	m_renderTarget->Clear(gfx);

	RenderJobPass::Render(gfx);
}