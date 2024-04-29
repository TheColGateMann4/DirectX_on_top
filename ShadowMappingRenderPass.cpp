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
	m_bindsGraphicBuffersByItself = true;
	m_depthStencilView = std::make_shared<DepthStencilView>(gfx, DepthStencilView::StencilAndDepth, true);
	depthTextureCube = std::make_shared<DepthTextureCube>(gfx, 3);

	{
		DynamicConstantBuffer::BufferLayout layout;

		layout.Add<DynamicConstantBuffer::DataType::Matrix>("shadowCameraView");

		DynamicConstantBuffer::BufferData bufferData(std::move(layout));

		shadowCameraTransformBuffer = std::make_shared<CachedBuffer>(gfx, bufferData, 1, false);
	}

	RegisterOutput(RenderPassBindableOutput<DepthTextureCube>::GetUnique("shadowMap", &depthTextureCube));
	RegisterOutput(RenderPassBindableOutput<CachedBuffer>::GetUnique("shadowCameraTransformBuffer", &shadowCameraTransformBuffer));

	AddBindable(BlendState::GetBindable(gfx, false));
	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::Off));
	AddBindable(PixelShader::GetBindable(gfx, "PS_Shadow.cso"));
}

void ShadowMappingRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	Camera* previousCamera = m_scene->GetCameraManager()->GetActiveCamera();
	ShadowCamera* shadowCamera = m_scene->GetLights().front()->GetShadowCamera();

	{
		DynamicConstantBuffer::BufferData bufferData = shadowCameraTransformBuffer->GetBufferData();

		DirectX::XMFLOAT3 position = shadowCamera->GetWorldPosition();

		DirectX::XMMATRIX cameraModel = DirectX::XMMatrixTranspose(
			DirectX::XMMatrixTranslation(-position.x, -position.y, -position.z)
		);

		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Matrix>("shadowCameraView") = cameraModel;

		shadowCameraTransformBuffer->Update(gfx, bufferData);
	}

	m_scene->GetCameraManager()->SetActiveCameraByPtr(shadowCamera);

	RenderFromAllAngles(gfx, shadowCamera);

	m_scene->GetCameraManager()->SetActiveCameraByPtr(previousCamera);
}

void ShadowMappingRenderPass::RenderModels(GFX& gfx) const noexcept(!IS_DEBUG)
{
	m_depthStencilView->Clear(gfx);

	RenderJobPass::Render(gfx);
}

void ShadowMappingRenderPass::RenderFromAllAngles(GFX& gfx, ShadowCamera* shadowCamera) const noexcept(!IS_DEBUG)
{
	//Right
	shadowCamera->Look({ _Pi / 2, 0.0f, 0.0f });
	depthTextureCube->BindDepthTextureCubeSide(gfx, CubeTextureDrawingOrder::Right, m_depthStencilView.get());
	RenderModels(gfx);

 	//Left
	shadowCamera->Look({ -_Pi / 2, 0.0f, 0.0f });
	depthTextureCube->BindDepthTextureCubeSide(gfx, CubeTextureDrawingOrder::Left, m_depthStencilView.get());
 	RenderModels(gfx);

 	//Up
	shadowCamera->SetUpVector({ 0.0f, 0.0f, -1.0f });
	shadowCamera->Look({ 0.0f, -_Pi / 2, 0.0f });
	depthTextureCube->BindDepthTextureCubeSide(gfx, CubeTextureDrawingOrder::Up, m_depthStencilView.get());
	RenderModels(gfx);
 
 	//Down
	shadowCamera->SetUpVector({ 0.0f, 0.0f, 1.0f });
	shadowCamera->Look({0.0f, _Pi / 2, 0.0f });
	depthTextureCube->BindDepthTextureCubeSide(gfx, CubeTextureDrawingOrder::Down, m_depthStencilView.get());
 	RenderModels(gfx);

	//Back
	shadowCamera->SetUpVector({ 0.0f, 1.0f, 0.0f });
	shadowCamera->Look({ -_Pi, 0.0f, 0.0f });
	depthTextureCube->BindDepthTextureCubeSide(gfx, CubeTextureDrawingOrder::Back, m_depthStencilView.get());
	RenderModels(gfx);

 	//Front
	shadowCamera->Look({ 0.0f , 0.0f, 0.0f });
	depthTextureCube->BindDepthTextureCubeSide(gfx, CubeTextureDrawingOrder::Front, m_depthStencilView.get());
 	RenderModels(gfx);
}