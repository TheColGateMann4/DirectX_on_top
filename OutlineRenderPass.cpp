#include "OutlineRenderPass.h"
#include "BindableClassesMacro.h"

OutlineRenderPass::OutlineRenderPass(class GFX& gfx, const char* name, const int width, const int height)
	: RenderJobPass(name)
{
	m_renderTarget = std::make_shared<RenderTargetWithTexture>(gfx, width / 2, height / 2, 0);

	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("pixelShaderTexture", &m_renderTarget));

	AddBindable(PixelShader::GetBindable(gfx, "PS.cso"));
	AddBindable(VertexShader::GetBindable(gfx, "VS.cso"));
	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Mask));
	AddBindable(BlendState::GetBindable(gfx, false));
}

void OutlineRenderPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	m_renderTarget->Clear(gfx);

	RenderJobPass::Render(gfx);
}

