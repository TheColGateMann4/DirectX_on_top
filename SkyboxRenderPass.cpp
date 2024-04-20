#include "SkyboxRenderPass.h"
#include "BindableClassesMacro.h"
#include "CubeMapTexture.h"
#include "SkyboxTransformBuffer.h"

SkyboxRenderPass::SkyboxRenderPass(class GFX& gfx, const char* name)
	:
	RenderJobPass(name)
{
	RegisterInput(std::make_unique<RenderPassBufferInput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterInput(std::make_unique<RenderPassBufferInput<DepthStencilView>>("depthStencilView", &m_depthStencilView));

	AddBindable(PixelShader::GetBindable(gfx, "PS_Skybox.cso"));
	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::RawDepth));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::CLAMP, 0, SamplerState::NEVER, SamplerState::BILINEAR));
	AddBindable(BlendState::GetBindable(gfx, false));

	RegisterOutput(std::make_unique<RenderPassBufferOutput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterOutput(std::make_unique<RenderPassBufferOutput<DepthStencilView>>("depthStencilView", &m_depthStencilView));
}