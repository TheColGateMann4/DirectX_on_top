#include "NormalRenderPass.h"
#include "RenderPassOuput.h"
#include "RenderPassInput.h"
#include "RenderTarget.h"
#include "BindableClassesMacro.h"
#include "imgui/imgui.h"
#include "DepthTextureCube.h"

#include "Camera.h"
#include "Scene.h"
#include "PointLight.h"
#include "ShadowCamera.h"

NormalRenderPass::NormalRenderPass(class GFX& gfx, const char* name)
	: 
	RenderJobPass(name)
{
	RegisterInput(std::make_unique<RenderPassBufferInput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterInput(std::make_unique<RenderPassBufferInput<DepthStencilView>>("depthStencilView", &m_depthStencilView));
	AddBindableInput<CachedBuffer>("shadowCameraTransformBuffer");
	AddBindableInput<DepthTextureCube>("shadowMap");
	AddBindableInput<CachedBuffer>("shadowCameraData");

	RegisterOutput(std::make_unique<RenderPassBufferOutput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterOutput(std::make_unique<RenderPassBufferOutput<DepthStencilView>>("depthStencilView", &m_depthStencilView));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Off));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::MIRROR, 0, SamplerState::NEVER, SamplerState::POINT));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 1, SamplerState::LESS_EQUAL, SamplerState::POINT));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 2, SamplerState::NEVER, SamplerState::POINT));
}

void NormalRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	RenderJobPass::Render(gfx);

	//unbinding depthStencilView texture after we used it where we needed it
	NullTexture::GetBindable(gfx, 3, true)->Bind(gfx);
}