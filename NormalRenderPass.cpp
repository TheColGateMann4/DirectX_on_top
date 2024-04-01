#include "NormalRenderPass.h"
#include "RenderPassOuput.h"
#include "RenderPassInput.h"
#include "RenderTarget.h"
#include "BindableClassesMacro.h"

NormalRenderPass::NormalRenderPass(class GFX& gfx, const char* name)
	: RenderJobPass(name)
{
	RegisterOutput(std::make_unique<RenderPassBufferNewOutput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterOutput(std::make_unique<RenderPassBufferNewOutput<DepthStencilView>>("depthStencilView", &m_depthStencilView));

	RegisterInput(std::make_unique<RenderPassBufferInput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterInput(std::make_unique<RenderPassBufferInput<DepthStencilView>>("depthStencilView", &m_depthStencilView));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Off));
	AddBindable(BlendState::GetBindable(gfx, false));
	AddBindable(SamplerState::GetBindable(gfx, true, false));
}