#include "NormalOutlineRenderGraph.h"
#include "Graphics.h"

#include "BufferClearRenderPass.h"
#include "NormalRenderPass.h"
#include "OutlineMaskingRenderPass.h"
#include "OutlineRenderPass.h"
#include "BoxBlurRenderPass.h"

NormalOutlineRenderGraph::NormalOutlineRenderGraph(GFX& gfx)
	: RenderGraph(gfx)
{
	{
		//cleaning up buffers after previous frame
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

		//drawing our regular meshes
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
			auto renderPass = std::make_unique<BoxBlurRenderPass>(gfx, "boxBlurPass");
			renderPass->LinkInput("pixelShaderTexture", "outlineMaskPass.pixelShaderTexture");
			renderPass->LinkInput("depthStencilView", "outlineWriteMaskPass.depthStencilView");
			renderPass->LinkInput("renderTarget", "normalPass.renderTarget");
			AddPass(std::move(renderPass));
		}
	}

	SetTarget("backBuffer", "boxBlurPass.renderTarget");
	Finish();
}