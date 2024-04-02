#include "BlurOutlineRenderPass.h"
#include "BindableClassesMacro.h"

BlurOutlineRenderPass::BlurOutlineRenderPass(class GFX& gfx, const char* name, float resolutionX, float resolutionY)
	: RenderJobPass(name)
{
	m_renderTarget = std::make_shared<RenderTargetWithTexture>(gfx, resolutionX / 2, resolutionY / 2, 0);

	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("renderTargetWithTexture", &m_renderTarget));

	AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));
	AddBindable(VertexShader::GetBindable(gfx, "VS.cso"));
	
	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Mask));
	AddBindable(BlendState::GetBindable(gfx, false));
}

void BlurOutlineRenderPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	m_renderTarget->Clear(gfx);
	RenderJobPass::Render(gfx);
}