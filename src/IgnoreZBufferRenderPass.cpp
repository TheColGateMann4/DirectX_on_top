#include "IgnoreZBufferRenderPass.h"
#include "BindableClassesMacro.h"

IgnoreZBufferRenderPass::IgnoreZBufferRenderPass(class GFX& gfx, const char* name)
	: RenderJobPass(name)
{
	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));

	RegisterInput(RenderPassBufferInput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::IgnoreZBuffer));
	AddBindable(BlendState::GetBindable(gfx, true));
}