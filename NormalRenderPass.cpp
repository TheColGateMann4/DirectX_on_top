#include "NormalRenderPass.h"
#include "RenderPassOuput.h"
#include "RenderPassInput.h"
#include "RenderTarget.h"
#include "BindableClassesMacro.h"

NormalRenderPass::NormalRenderPass(class GFX& gfx, const char* name)
	: RenderJobPass(name)
{
	RegisterInput(std::make_unique<RenderPassBufferInput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterInput(std::make_unique<RenderPassBufferInput<DepthStencilView>>("depthStencilView", &m_depthStencilView));
	AddBindableInput<DepthStencilViewWithTexture>("shadowMap");
	AddBindableInput<CachedBuffer>("shadowCameraTransformBuffer");

	RegisterOutput(std::make_unique<RenderPassBufferOutput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterOutput(std::make_unique<RenderPassBufferOutput<DepthStencilView>>("depthStencilView", &m_depthStencilView));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Off));
	AddBindable(BlendState::GetBindable(gfx, false));
	AddBindable(SamplerState::GetBindable(gfx, true, SamplerState::Mode::CLAMP));
}

void NormalRenderPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	RenderJobPass::Render(gfx);

	//unbinding depthStencilView texture after we used it where we needed it
	NullTexture::GetBindable(gfx, 2, true)->Bind(gfx);
}