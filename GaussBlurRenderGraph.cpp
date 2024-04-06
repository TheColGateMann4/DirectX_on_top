#include "GaussBlurRenderGraph.h"
#include "RenderPassOuput.h"

#include "BufferClearRenderPass.h"
#include "NormalRenderPass.h"
#include "OutlineMaskingRenderPass.h"
#include "OutlineRenderPass.h"
#include "HorizontalGaussBlurRenderPass.h"
#include "VerticalGaussBlurRenderPass.h"
#include "IgnoreZBufferRenderPass.h"

GaussBlurRenderGraph::GaussBlurRenderGraph(class GFX& gfx)
	: RenderGraph(gfx)
{
	{
		{
			auto renderPass = std::make_unique<BufferClearRenderPass>(gfx, "clearBackBuffer");
			renderPass->LinkInput("buffer", "$.backBuffer");
			AddPass(std::move(renderPass));
		}

		{
			auto renderPass = std::make_unique<BufferClearRenderPass>(gfx, "clearDepthStencilView");
			renderPass->LinkInput("buffer", "$.depthStencilView");
			AddPass(std::move(renderPass));
		}

		{
			auto renderPass = std::make_unique<NormalRenderPass>(gfx, "normalPass");
			renderPass->LinkInput("depthStencilView", "clearDepthStencilView.buffer");
			renderPass->LinkInput("renderTarget", "clearBackBuffer.buffer");
			AddPass(std::move(renderPass));
		}

		{
			auto renderPass = std::make_unique<OutlineMaskingRenderPass>(gfx, "outlineWriteMaskPass");
			renderPass->LinkInput("depthStencilView", "normalPass.depthStencilView");
			AddPass(std::move(renderPass));
		}

		{
			auto renderPass = std::make_unique<OutlineRenderPass>(gfx, "outlineMaskPass", gfx.GetWidth(), gfx.GetHeight());
			AddPass(std::move(renderPass));
		}

		{
			{
				DynamicConstantBuffer::BufferLayout constBufferLayout;
				constBufferLayout.Add<DynamicConstantBuffer::DataType::Int>("numberUsed");
				constBufferLayout.Add<DynamicConstantBuffer::DataType::Array>("cooficients");
				constBufferLayout.SetArrayAttributes("cooficients", DynamicConstantBuffer::DataType::Float4, 15); // we gotta use float4 to use single float value in array because of hlsl alignment

				DynamicConstantBuffer::BufferData constBufferData(constBufferLayout);


				cooficientSettings = std::make_shared<CachedBuffer>(gfx, constBufferData, 0, true);

				SetCooficients(gfx, 7, 2.6f, &cooficientSettings);

				AddGlobalOutput(RenderPassBindableOutput<CachedBuffer>::GetUnique("gaussCooficientSettings", &cooficientSettings));
			}

			{
				DynamicConstantBuffer::BufferLayout constBufferLayout;
				constBufferLayout.Add<DynamicConstantBuffer::DataType::Bool>("horizontal");

				DynamicConstantBuffer::BufferData constBufferData(constBufferLayout);
				*constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("horizontal") = true;


				directionSettings = std::make_shared<CachedBuffer>(gfx, constBufferData, 1, true);

				AddGlobalOutput(RenderPassBindableOutput<CachedBuffer>::GetUnique("gaussDirectionSettings", &directionSettings));
			}
		}

		{
			auto renderPass = std::make_unique<HorizontalGaussBlurRenderPass>(gfx, "horizontalGaussBlurPass", gfx.GetWidth(), gfx.GetHeight());
			renderPass->LinkInput("gaussCooficientSettings", "$.gaussCooficientSettings");
			renderPass->LinkInput("gaussDirectionSettings", "$.gaussDirectionSettings");
			renderPass->LinkInput("pixelShaderTexture", "outlineMaskPass.pixelShaderTexture");

			AddPass(std::move(renderPass));
		}

		{
			auto renderPass = std::make_unique<VerticalGaussBlurRenderPass>(gfx, "verticalGaussBlurPass");
			renderPass->LinkInput("gaussCooficientSettings", "$.gaussCooficientSettings");
			renderPass->LinkInput("gaussDirectionSettings", "$.gaussDirectionSettings");
			renderPass->LinkInput("pixelShaderTexture", "horizontalGaussBlurPass.pixelShaderTexture");
			renderPass->LinkInput("renderTarget", "normalPass.renderTarget");
			renderPass->LinkInput("depthStencilView", "outlineWriteMaskPass.depthStencilView");
			AddPass(std::move(renderPass));
		}

		{
			auto renderPass = std::make_unique<IgnoreZBufferRenderPass>(gfx, "ignoreZBufferPass");
			renderPass->LinkInput("renderTarget", "verticalGaussBlurPass.renderTarget");
			AddPass(std::move(renderPass));
		}
	}

	SetTarget("backBuffer", "ignoreZBufferPass.renderTarget");
	Finish();
}

void GaussBlurRenderGraph::SetCooficients(class GFX& gfx, int range, float sigma, std::shared_ptr<CachedBuffer>* cooficientSettings) noexcept
{
	assert(range <= 14);
	
	// sets linked to true + we are changing copy instead of original thing
	DynamicConstantBuffer::BufferData bufferData = cooficientSettings->get()->GetBufferData();

	int* numberUsed = bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("numberUsed");

	*numberUsed = range;

	float sum = 0.0f;
	for (int i = 0; i <= *numberUsed; i++)
	{
		const size_t cooficientIndex = std::abs(i - range);
		const float gaussDensity = CalculateGaussDensity(i - range, sigma);

		if (cooficientIndex > 0)
			sum += gaussDensity * 2;
		else
			sum += gaussDensity;

		float* cooficientValue = static_cast<float*>(bufferData.GetArrayDataPointerValue<DynamicConstantBuffer::DataType::Float4>("cooficients", cooficientIndex));
		*cooficientValue = gaussDensity;
	}

	for (int i = 0; i <= *numberUsed; i++)
	{
		float* cooficientValue = static_cast<float*>(bufferData.GetArrayDataPointerValue<DynamicConstantBuffer::DataType::Float4>("cooficients", i));

		*cooficientValue /= sum;
	}

	cooficientSettings->get()->Update(gfx, bufferData);
}

float GaussBlurRenderGraph::CalculateGaussDensity(const float x, const float sigma) const noexcept
{
	const auto sigmaSquared = sigma * sigma;
	return (1.0f / std::sqrt(2.0f * _Pi * sigmaSquared)) * std::exp(-(x * x) / (2.0f * sigmaSquared));
}