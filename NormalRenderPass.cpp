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
	RegisterInput(RenderPassBufferInput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));
	RegisterInput(RenderPassBufferInput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));
	AddBindableInput<CachedBuffer>("shadowCameraTransformBuffer");
	AddBindableInput<DepthTextureCube>("shadowMap");
	AddBindableInput<CachedBuffer>("shadowCameraData");

	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));
	RegisterOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));

	AddBindable(BlendState::GetBindable(gfx, false));
	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Off));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::MIRROR, 0, SamplerState::NEVER, SamplerState::POINT));
	//AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 1, SamplerState::LESS_EQUAL, SamplerState::BILINEAR));
}

void NormalRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 1, SamplerState::LESS_EQUAL, (shadowTextureMultiSampling) ? SamplerState::BILINEAR : SamplerState::POINT)->Bind(gfx);
	SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 2, SamplerState::NEVER, (shadowTextureMultiSampling) ? SamplerState::BILINEAR : SamplerState::POINT)->Bind(gfx);
	SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 3, SamplerState::NEVER, SamplerState::POINT)->Bind(gfx);

	m_scene->UpdateSceneVisibility(gfx);

	// render call for our regular camera
	RenderJobPass::Render(gfx);

	//unbinding depthStencilView texture after we used it where we needed it
	NullTexture::GetBindable(gfx, 6, true)->Bind(gfx);
}

void NormalRenderPass::ShowWindow(GFX& gfx, bool show)
{
	if (!show)
		return;

	if (ImGui::Begin("Shadow Settings"))
	{
		ImGui::Checkbox("BillinearSampling", &shadowTextureMultiSampling);
	}

	ImGui::End();
}