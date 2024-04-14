#include "NormalRenderPass.h"
#include "RenderPassOuput.h"
#include "RenderPassInput.h"
#include "RenderTarget.h"
#include "BindableClassesMacro.h"
#include "imgui/imgui.h"

NormalRenderPass::NormalRenderPass(class GFX& gfx, const char* name)
	: 
	RenderJobPass(name),
	samplerStateManager(gfx)
{
	RegisterInput(std::make_unique<RenderPassBufferInput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterInput(std::make_unique<RenderPassBufferInput<DepthStencilView>>("depthStencilView", &m_depthStencilView));
	AddBindableInput<DepthStencilViewWithTexture>("shadowMap");
	AddBindableInput<CachedBuffer>("shadowCameraTransformBuffer");

	RegisterOutput(std::make_unique<RenderPassBufferOutput<RenderTarget>>("renderTarget", &m_renderTarget));
	RegisterOutput(std::make_unique<RenderPassBufferOutput<DepthStencilView>>("depthStencilView", &m_depthStencilView));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Off));
	AddBindable(BlendState::GetBindable(gfx, false));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 0, SamplerState::NEVER, SamplerState::ANISOTROPIC));

	//objects with normal mesh use only these two, so we can dirty override them
	shadowRasterizerNoBackculling = RasterizerState::GetBindable(gfx, true);
	shadowRasterizerWithculling = RasterizerState::GetBindable(gfx, false);

	{

		DynamicConstantBuffer::BufferLayout layout;

		DynamicConstantBuffer::ImguiAdditionalInfo::ImguiIntInfo intInfo = {};
		intInfo.v_min = 0;
		intInfo.v_max = 4;

		DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo floatInfo = {};
		floatInfo.v_min = 0.0f;
		floatInfo.v_max = 0.005f;
		floatInfo.format = "%.6f";

		layout.Add<DynamicConstantBuffer::DataType::Int>("PCF_level", &intInfo);
		layout.Add<DynamicConstantBuffer::DataType::Float>("bias", &floatInfo);
		layout.Add<DynamicConstantBuffer::DataType::Bool>("hardwarePCF");
		layout.Add<DynamicConstantBuffer::DataType::Bool>("bilinear");

		DynamicConstantBuffer::BufferData bufferData(std::move(layout));

		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("PCF_level") = 0;
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("bias") = 0.0005f;
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("hardwarePCF") = FALSE;
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("bilinear") = FALSE;

		shadowSettings = std::make_shared<CachedBuffer>(gfx, bufferData, 4, true);
	}

	AddBindable(shadowSettings);
}

void NormalRenderPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	samplerStateManager.Bind(gfx);

	RenderJobPass::Render(gfx);

	//unbinding depthStencilView texture after we used it where we needed it
	NullTexture::GetBindable(gfx, 2, true)->Bind(gfx);
}

void NormalRenderPass::ShowWindow(GFX& gfx)
{
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
				checkChanged(ImGui::SliderFloat("hBiasClamp", &biasClamp, 0.00001f, 1.0f, "%.5f"));
				checkChanged(ImGui::SliderFloat("hSlopeScaledDepthBias", &slopeScaledDepthBias, 0.0f, 100.0f, "%.5f"));
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