#include "ShadowMappingRenderPass.h"
#include "RenderPassInput.h"
#include "RenderPassOuput.h"
#include "BindableClassesMacro.h"
#include "Scene.h"
#include "Pointlight.h"
#include "Camera.h"
#include "ShadowCamera.h"
#include "CubeTextureDrawingOrder.h"

ShadowMappingRenderPass::ShadowMappingRenderPass(GFX& gfx, const char* name)
	:
	RenderJobPass(name)
{
	//m_bindsGraphicBuffersByItself = true;
	m_renderTarget = std::make_shared<RenderTarget>(gfx, gfx.GetWidth(), gfx.GetWidth());
	m_depthStencilView = std::make_shared<DepthStencilView>(gfx, DepthStencilView::DepthOnly, true);

	{
		DynamicConstantBuffer::BufferLayout layout;

		layout.Add<DynamicConstantBuffer::DataType::Matrix>("shadowCameraView");

		DynamicConstantBuffer::BufferData bufferData(std::move(layout));

		shadowCameraTransformBuffer = std::make_shared<CachedBuffer>(gfx, bufferData, 1, false);
	}

	//RegisterOutput(RenderPassBindableOutput<DepthTextureCube>::GetUnique("shadowMap", &depthTextureCube));
	RegisterOutput(RenderPassBindableOutput<CachedBuffer>::GetUnique("shadowCameraTransformBuffer", &shadowCameraTransformBuffer));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::Off));
}

void ShadowMappingRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	Camera* previousCamera = m_scene->GetCameraManager()->GetActiveCamera();
	ShadowCamera* shadowCamera = m_scene->GetLights().front()->GetShadowCamera();

	{
		DynamicConstantBuffer::BufferData bufferData = shadowCameraTransformBuffer->GetBufferData();

		DirectX::XMMATRIX shadowCameraView = shadowCamera->GetCameraView();
		DirectX::XMMATRIX shadowCameraProjection = shadowCamera->GetProjection();

		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Matrix>("shadowCameraView") = DirectX::XMMatrixTranspose(shadowCameraView * shadowCameraProjection);

		shadowCameraTransformBuffer->Update(gfx, bufferData);
	}

	m_scene->GetCameraManager()->SetActiveCameraByPtr(shadowCamera);

	RenderFromAllAngles(gfx, shadowCamera);

	m_scene->GetCameraManager()->SetActiveCameraByPtr(previousCamera);
}

void ShadowMappingRenderPass::RenderModels(GFX& gfx) const noexcept(!IS_DEBUG)
{
	//we are not clearning m_depthStencilView since we don't use it. We are using depthStencils from shadowCamera
	m_depthStencilView->Clear(gfx);
	m_renderTarget->Clear(gfx);

	RenderJobPass::Render(gfx);
}

void ShadowMappingRenderPass::RenderFromAllAngles(GFX& gfx, ShadowCamera* shadowCamera) const noexcept(!IS_DEBUG)
{
	//front
	//shadowCamera->SetCurrentCubeDrawingIndex(gfx, CubeTextureDrawingOrder::Front, m_renderTarget.get());
	RenderModels(gfx);

// 	//Back
 	shadowCamera->Look({ -_Pi, 0.0f, 0.0f });
// 	shadowCamera->SetCurrentCubeDrawingIndex(gfx, CubeTextureDrawingOrder::Back, m_renderTarget.get());
 	RenderModels(gfx);
// 
// 	//Up
//	shadowCamera->Look({ -_Pi, _Pi / 2, 0.0f });
	shadowCamera->Look({ 0.0f, _Pi / 2, 0.0f });
// 	shadowCamera->SetCurrentCubeDrawingIndex(gfx, CubeTextureDrawingOrder::Up, m_renderTarget.get());
	RenderModels(gfx);
// 
// 	//Down
// 	shadowCamera->Look({ 0.0f, -_Pi, 0.0f });
 	shadowCamera->Look({ 0.0f, -_Pi / 2, 0.0f });
// 	shadowCamera->SetCurrentCubeDrawingIndex(gfx, CubeTextureDrawingOrder::Down, m_renderTarget.get());
 	RenderModels(gfx);
// 
// 	//Left
//	shadowCamera->Look({ -_Pi / 2, _Pi / 2, 0.0f });
	shadowCamera->Look({ -_Pi / 2, 0.0f, 0.0f });
// 	shadowCamera->SetCurrentCubeDrawingIndex(gfx, CubeTextureDrawingOrder::Left, m_renderTarget.get());
 	RenderModels(gfx);
// 
// 	//Right
// 	shadowCamera->Look({ _Pi, 0.0f, 0.0f });
 	shadowCamera->Look({ _Pi / 2, 0.0f, 0.0f });
// 	shadowCamera->SetCurrentCubeDrawingIndex(gfx, CubeTextureDrawingOrder::Right, m_renderTarget.get());
 	RenderModels(gfx);
// 
// 	//leaving how it was
// 	shadowCamera->Look({ -_Pi / 2, 0.0f, 0.0f });
 	shadowCamera->Look({ 0.0f, 0.0f, 0.0f });
}