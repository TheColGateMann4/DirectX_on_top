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
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 1, SamplerState::LESS_EQUAL, SamplerState::BILINEAR));
}

void NormalRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	// rendering selected camera preview
	{
		m_previewCameraTexture->Clear(gfx, {0.0f, 0.0f, 0.0f, 1.0f});
		m_depthStencilView->Clear(gfx);
		m_bindsGraphicBuffersByItself = true;

		Camera* selectedCamera = m_scene->GetCameraManager()->GetSelectedCamera();

		if (selectedCamera != nullptr)
		{
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
					imageSize.x = 1600.0f / 4;
					imageSize.y = 900.0f / 4;
					ImGui::Image((void*)m_previewCameraTexture->GetSRV(), imageSize);
				}
				ImGui::End();
			}
		
			m_scene->GetCameraManager()->SetActiveCameraByPtr(previousCamera);
		}

		m_bindsGraphicBuffersByItself = false;
		m_depthStencilView->Clear(gfx);
	}

	m_scene->UpdateSceneVisibility(gfx);

	// render call for our regular camera
	RenderJobPass::Render(gfx);

	//unbinding depthStencilView texture after we used it where we needed it
	NullTexture::GetBindable(gfx, 6, true)->Bind(gfx);
}