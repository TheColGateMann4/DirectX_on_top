#include "ShadowMappingRenderPass.h"
#include "RenderPassInput.h"
#include "RenderPassOuput.h"
#include "BindableClassesMacro.h"
#include "Scene.h"
#include "Pointlight.h"
#include "Camera.h"

ShadowMappingRenderPass::ShadowMappingRenderPass(GFX& gfx, const char* name, class Scene& scene)
	:
	RenderJobPass(name),
	m_scene(&scene)
{
	m_depthStencilView = std::make_shared<DepthStencilViewWithTexture>(gfx, 3,DepthStencilView::Mode::DepthOnly);
	m_renderTarget = std::make_shared<RenderTarget>(gfx, gfx.GetWidth(), gfx.GetHeight());

	{
		DynamicConstantBuffer::BufferLayout layout;

		layout.Add<DynamicConstantBuffer::DataType::Matrix>("shadowCameraView");

		DynamicConstantBuffer::BufferData bufferData(std::move(layout));

		shadowCameraTransformBuffer = std::make_shared<CachedBuffer>(gfx, bufferData, 1, false);
	}

	RegisterOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("shadowMap", &m_depthStencilView));
	RegisterOutput(RenderPassBindableOutput<CachedBuffer>::GetUnique("shadowCameraTransformBuffer", &shadowCameraTransformBuffer));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::Off));
}

void ShadowMappingRenderPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	Camera* previousCamera = m_scene->GetCameraManager()->GetActiveCamera();
	Camera* shadowCamera = m_scene->GetLights().front()->GetShadowCamera();

	{
		DynamicConstantBuffer::BufferData bufferData = shadowCameraTransformBuffer->GetBufferData();

		DirectX::XMMATRIX shadowCameraView = shadowCamera->GetCameraView();
		DirectX::XMMATRIX shadowCameraProjection = shadowCamera->GetProjection();

		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Matrix>("shadowCameraView") = DirectX::XMMatrixTranspose(shadowCameraView * shadowCameraProjection);

		shadowCameraTransformBuffer->Update(gfx, bufferData);
	}

	m_scene->GetCameraManager()->SetActiveCameraByPtr(shadowCamera);

	{
		m_depthStencilView->Clear(gfx);
		m_renderTarget->Clear(gfx);

		RenderJobPass::Render(gfx);
	}

	m_scene->GetCameraManager()->SetActiveCameraByPtr(previousCamera);
}