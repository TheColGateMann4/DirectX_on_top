#include "NormalRenderPass.h"
#include "RenderPassOuput.h"
#include "RenderPassInput.h"
#include "RenderTarget.h"
#include "BindableClassesMacro.h"
#include "imgui/imgui.h"
#include "DepthTextureCube.h"

NormalRenderPass::NormalRenderPass(class GFX& gfx, const char* name)
	: 
	RenderJobPass(name),
	samplerStateManager(gfx)
{
	RegisterInput(std::make_unique<RenderPassBufferInput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterInput(std::make_unique<RenderPassBufferInput<DepthStencilView>>("depthStencilView", &m_depthStencilView));
	AddBindableInput<CachedBuffer>("shadowCameraTransformBuffer");
	AddBindableInput<DepthTextureCube>("shadowMap");

	RegisterOutput(std::make_unique<RenderPassBufferOutput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterOutput(std::make_unique<RenderPassBufferOutput<DepthStencilView>>("depthStencilView", &m_depthStencilView));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Off));
	AddBindable(BlendState::GetBindable(gfx, false));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::MIRROR, 0, SamplerState::NEVER, SamplerState::BILINEAR));

	{

		DynamicConstantBuffer::BufferLayout layout;

		DynamicConstantBuffer::ImguiAdditionalInfo::ImguiIntInfo pcfLevelInfo = {};
		pcfLevelInfo.v_min = 0;
		pcfLevelInfo.v_max = 4;

		DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo biasInfo = {};
		biasInfo.v_min = 0.0f;
		biasInfo.v_max = 0.005f;
		biasInfo.format = "%.6f";

		DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo radiusInfo = {};
		radiusInfo.v_min = 0.01f;
		radiusInfo.v_max = 1.0f;
		radiusInfo.format = "%.2f";

		layout.Add<DynamicConstantBuffer::DataType::Int>("PCF_level", &pcfLevelInfo);
		layout.Add<DynamicConstantBuffer::DataType::Float>("bias", &biasInfo);
		layout.Add<DynamicConstantBuffer::DataType::Bool>("hardwarePCF");
		layout.Add<DynamicConstantBuffer::DataType::Bool>("bilinear");
		layout.Add<DynamicConstantBuffer::DataType::Bool>("circleFilter");
		layout.Add<DynamicConstantBuffer::DataType::Float>("radius", &radiusInfo);

		DynamicConstantBuffer::BufferData bufferData(std::move(layout));

		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("PCF_level") = 0;
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("bias") = 0.0f;
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("hardwarePCF") = FALSE;
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("circleFilter") = FALSE;
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("radius") = 1.0f;

		shadowSettings = std::make_shared<CachedBuffer>(gfx, bufferData, 4, true);
	}

	AddBindable(shadowSettings);


	//objects with normal mesh use only these two, so we can dirty override them
	shadowRasterizerNoBackculling = RasterizerState::GetBindable(gfx, true);
	shadowRasterizerWithculling = RasterizerState::GetBindable(gfx, false);

	shadowRasterizerNoBackculling->ChangeDepthValues(gfx, bias, biasClamp, slopeScaledDepthBias);
	shadowRasterizerWithculling->ChangeDepthValues(gfx, bias, biasClamp, slopeScaledDepthBias);
}

void NormalRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	samplerStateManager.Bind(gfx);

	RenderJobPass::Render(gfx);

	//unbinding depthStencilView texture after we used it where we needed it
	NullTexture::GetBindable(gfx, 3, true)->Bind(gfx);
}

void NormalRenderPass::ShowWindow(GFX& gfx, bool show)
{
	if (!show)
		return;

	if (ImGui::Begin("Shadow Settings"))
	{
		DynamicConstantBuffer::BufferData bufferData = shadowSettings->GetBufferData();

		bool bufferChanged = bufferData.MakeImguiMenu();
			
		if(bufferChanged)
			shadowSettings->Update(gfx, bufferData);

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

				ImGui::Text("Hardware Varibles");

				checkChanged(ImGui::SliderInt("hBias", &bias, 0, 1000000));
			}

			if (changed)
			{
				shadowRasterizerNoBackculling->ChangeDepthValues(gfx, bias, biasClamp, slopeScaledDepthBias);
				shadowRasterizerWithculling->ChangeDepthValues(gfx, bias, biasClamp, slopeScaledDepthBias);
			}
		}

		if (bufferChanged)
		{
			bool hardwarePCF = *bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("hardwarePCF");
			bool bilinear = *bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("bilinear");

			samplerStateManager.SetActiveSampler(hardwarePCF, bilinear);
		}

		ImGui::End();
	}
}