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
	m_renderTarget = std::make_shared<RenderTarget>(gfx, gfx.GetWidth(), gfx.GetWidth());
	depthTextureCube = std::make_shared<DepthTextureCube>(gfx, 6);

	{
		DynamicConstantBuffer::BufferLayout layout;

		layout.Add<DynamicConstantBuffer::DataType::Matrix>("shadowCameraView");

		DynamicConstantBuffer::BufferData bufferData(std::move(layout));

		shadowCameraTransformBuffer = CachedBuffer::GetBindable(gfx, bufferData, { {TargetVertexShader, 1}, {TargetDomainShader, 2} });
	} 

	{
		DynamicConstantBuffer::BufferLayout layout;

		layout.Add<DynamicConstantBuffer::DataType::Float>("c0");
		layout.Add<DynamicConstantBuffer::DataType::Float>("c1");
		layout.Add<DynamicConstantBuffer::DataType::Int>("pcf");

		DynamicConstantBuffer::BufferData bufferData(std::move(layout));

		shadowCameraData = CachedBuffer::GetBindable(gfx, bufferData, {{TargetPixelShader, 4}});
	}

	RegisterOutput(RenderPassBindableOutput<DepthTextureCube>::GetUnique("shadowMap", &depthTextureCube));
	RegisterOutput(RenderPassBindableOutput<CachedBuffer>::GetUnique("shadowCameraTransformBuffer", &shadowCameraTransformBuffer));
	RegisterOutput(RenderPassBindableOutput<CachedBuffer>::GetUnique("shadowCameraData", &shadowCameraData));
	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::Off));
	AddBindable(NullShader::GetBindable(gfx, TargetPixelShader));
	shadowRasterizer = std::make_unique<RasterizerState>(gfx, true, bias, biasClamp, slopeScaledDepthBias);
}

void ShadowMappingRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	Camera* previousCamera = m_scene->GetCameraManager()->GetActiveCamera();
	ShadowCamera* shadowCamera = m_scene->GetLights().front()->GetShadowCamera();

	UpdateCameraData(gfx, shadowCamera);

	UpdateCameraTransformBuffer(gfx, shadowCamera);

	m_scene->GetCameraManager()->SetActiveCameraByPtr(shadowCamera);

	RenderFromAllAngles(gfx, shadowCamera);

	m_scene->GetCameraManager()->SetActiveCameraByPtr(previousCamera);
}

void ShadowMappingRenderPass::RenderModels(GFX& gfx, std::shared_ptr<DepthStencilView> sideDepthStencilView) const noexcept(!IS_DEBUG)
{
	m_depthStencilView = sideDepthStencilView;
	m_renderTarget->BindRenderTarget(gfx, m_depthStencilView.get());
	m_renderTarget->Clear(gfx);
	m_depthStencilView->Clear(gfx);

	shadowRasterizer->Bind(gfx);

	m_scene->UpdateSceneVisibility(gfx);

	m_captureFrame = m_captureFrameAngles;

	RenderJobPass::Render(gfx);
}

void ShadowMappingRenderPass::RenderFromAllAngles(GFX& gfx, ShadowCamera* shadowCamera) const noexcept(!IS_DEBUG)
{
	m_captureFrameAngles = m_captureFrame;

	//Right
	shadowCamera->Look({ _Pi / 2, 0.0f, 0.0f });
	RenderModels(gfx, depthTextureCube->GetDepthTextureCubeSide(CubeTextureDrawingOrder::Right));

 	//Left
	shadowCamera->Look({ -_Pi / 2, 0.0f, 0.0f });
 	RenderModels(gfx, depthTextureCube->GetDepthTextureCubeSide(CubeTextureDrawingOrder::Left));

 	//Up
	shadowCamera->SetUpVector({ 0.0f, 0.0f, -1.0f });
	shadowCamera->Look({ 0.0f, -_Pi / 2, 0.0f });
	RenderModels(gfx, depthTextureCube->GetDepthTextureCubeSide(CubeTextureDrawingOrder::Up));
 
 	//Down
	shadowCamera->SetUpVector({ 0.0f, 0.0f, 1.0f });
	shadowCamera->Look({0.0f, _Pi / 2, 0.0f });
 	RenderModels(gfx, depthTextureCube->GetDepthTextureCubeSide(CubeTextureDrawingOrder::Down));

	//Back
	shadowCamera->SetUpVector({ 0.0f, 1.0f, 0.0f });
	shadowCamera->Look({ -_Pi, 0.0f, 0.0f });
	RenderModels(gfx, depthTextureCube->GetDepthTextureCubeSide(CubeTextureDrawingOrder::Back));

 	//Front
	shadowCamera->Look({ 0.0f , 0.0f, 0.0f });
 	RenderModels(gfx, depthTextureCube->GetDepthTextureCubeSide(CubeTextureDrawingOrder::Front));
}

void ShadowMappingRenderPass::UpdateCameraData(GFX& gfx, ShadowCamera* shadowCamera) const
{
	DynamicConstantBuffer::BufferData bufferData = shadowCameraData->GetBufferData();

	CameraSettings cameraSettings;
	shadowCamera->GetCameraSettings(&cameraSettings);

	float nearZ = cameraSettings.m_NearZ;
	float farZ = cameraSettings.m_FarZ;


	if(cameraDataInitialized)
		if (nearZ == prevNearZ && farZ == prevFarZ && pcf == prevPCF)
			return;

	// Setting cameraDataInitialized to true and updating prev camera Z could have been done somewhere besides const functions. But this is way more clear.
	// Making those members mutable makes the code more inconsistent but its right in this case since its used to remove some ununnecessary looping and updating const buffer
	prevNearZ = nearZ;
	prevFarZ = farZ;
	prevPCF = pcf;
	cameraDataInitialized = true;

	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("c0") = -nearZ * farZ / (farZ - nearZ);
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("c1") = farZ / (farZ - nearZ);
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("pcf") = pcf;

	shadowCameraData->Update(gfx, bufferData);

}

void ShadowMappingRenderPass::UpdateCameraTransformBuffer(GFX& gfx, ShadowCamera* shadowCamera) const
{
	DynamicConstantBuffer::BufferData bufferData = shadowCameraTransformBuffer->GetBufferData();

	DirectX::XMFLOAT3 position = shadowCamera->GetWorldPosition();

	DirectX::XMMATRIX cameraModel = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixTranslation(-position.x, -position.y, -position.z)
	);

	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Matrix>("shadowCameraView") = cameraModel;

	shadowCameraTransformBuffer->Update(gfx, bufferData);
}

void ShadowMappingRenderPass::ShowWindow(GFX& gfx, bool show)
{
	if (!show)
		return;

	if (ImGui::Begin("Shadow Settings"))
	{
		bool changed = false;

		auto checkChanged = [&changed](bool returnFromStatement) mutable
			{
				changed = changed || returnFromStatement;
			};

		{
			DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo floatInfo = {};
			floatInfo.v_min = 0.0f;
			floatInfo.v_max = 0.005f;
			floatInfo.format = "%.6f";

			checkChanged(ImGui::SliderInt("pcf", &pcf, 0, 5));

			ImGui::Text("Hardware Varibles");

			checkChanged(ImGui::SliderInt("hBias", &bias, 0, 1000000));
			checkChanged(ImGui::SliderFloat("hBiasClamp", &biasClamp, 0.00001f, 1.0f, "%.5f"));
			checkChanged(ImGui::SliderFloat("hSlopeScaledDepthBias", &slopeScaledDepthBias, 0.0f, 100.0f, "%.5f"));
		}

		if (changed)
			shadowRasterizer->ChangeDepthValues(gfx, bias, biasClamp, slopeScaledDepthBias);
	}

	ImGui::End();
}