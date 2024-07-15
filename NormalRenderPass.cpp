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
	//AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 1, SamplerState::LESS_EQUAL, SamplerState::BILINEAR));

	m_previewCameraTexture = std::make_unique<RenderTargetWithTexture>(gfx, gfx.GetWidth(), gfx.GetHeight(), 0);
}

void NormalRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 1, SamplerState::LESS_EQUAL, (shadowTextureMultiSampling) ? SamplerState::BILINEAR : SamplerState::POINT)->Bind(gfx);
	SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 2, SamplerState::NEVER, (shadowTextureMultiSampling) ? SamplerState::BILINEAR : SamplerState::POINT)->Bind(gfx);
	SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 3, SamplerState::NEVER, SamplerState::POINT)->Bind(gfx);

	// rendering selected camera preview
	{
		Camera* selectedCamera = m_scene->GetCameraManager()->GetSelectedCamera();

		if (selectedCamera != nullptr)
		{
			m_previewCameraTexture->Clear(gfx, { 0.0f, 0.0f, 0.0f, 1.0f });
			m_depthStencilView->Clear(gfx);
			m_bindsGraphicBuffersByItself = true;

			Camera* previousCamera = m_scene->GetCameraManager()->GetActiveCamera();

			m_scene->GetCameraManager()->SetActiveCameraByPtr(selectedCamera);

			m_scene->UpdateSceneVisibility(gfx);

			m_previewCameraTexture.get()->BindRenderTarget(gfx, m_depthStencilView.get());
			BlendState::GetBindable(gfx, false)->Bind(gfx);
			RenderJobPass::Render(gfx);

			{
				ImGuiWindowFlags windowFlags =
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_AlwaysAutoResize;

				if (ImGui::Begin("Camera Preview", nullptr, windowFlags))
				{
					ImVec2 imageSize;
					imageSize.x = gfx.GetWidth() / 4;
					imageSize.y = gfx.GetHeight() / 4;
					ImGui::Image((void*)m_previewCameraTexture->GetSRV(), imageSize);
				}
				ImGui::End();
			}
		
			m_scene->GetCameraManager()->SetActiveCameraByPtr(previousCamera);
		
			m_bindsGraphicBuffersByItself = false;
			m_depthStencilView->Clear(gfx);
		}
	}

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