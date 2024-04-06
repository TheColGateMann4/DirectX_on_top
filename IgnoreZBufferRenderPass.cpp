#include "IgnoreZBufferRenderPass.h"
#include "BindableClassesMacro.h"

IgnoreZBufferRenderPass::IgnoreZBufferRenderPass(class GFX& gfx, const char* name)
	: RenderJobPass(name)
{
	RegisterOutput(std::make_unique<RenderPassBufferOutput<RenderTarget>>("renderTarget", &m_renderTarget));

	RegisterInput(std::make_unique<RenderPassBufferInput<RenderTarget>>("renderTarget", &m_renderTarget));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::IgnoreZBuffer));
	AddBindable(BlendState::GetBindable(gfx, true));
}