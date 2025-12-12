#include "SkyboxRenderPass.h"
#include "BindableClassesMacro.h"
#include "Bindable/CubeMapTexture.h"
#include "Bindable/SkyboxTransformBuffer.h"

SkyboxRenderPass::SkyboxRenderPass(class GFX& gfx, const char* name)
	:
	RenderJobPass(name)
{
	RegisterInput(RenderPassBufferInput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));
	RegisterInput(RenderPassBufferInput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));

	AddBindable(PixelShader::GetBindable(gfx, "PS_Skybox.cso"));
	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::RawDepth));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::CLAMP, 0, SamplerState::NEVER, SamplerState::BILINEAR));
	AddBindable(BlendState::GetBindable(gfx, false));

	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));
	RegisterOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));
}