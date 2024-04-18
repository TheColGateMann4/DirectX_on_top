#include "OutlineScalingRenderPass.h"
#include "BindableClassesMacro.h"
#include "RenderTarget.h"

OutlineScalingRenderPass::OutlineScalingRenderPass(class GFX& gfx, const char* name, const int width, const int height, bool horizontal)
	:
	FullscreenFilterRenderPass(gfx, name)
{
	m_renderTarget = std::make_shared<RenderTargetWithTexture>(gfx, width / 2, height / 2, 0);

	AddBindableInput<RenderTargetWithTexture>("pixelShaderTexture");

	{
		DynamicConstantBuffer::BufferLayout bufferLayout;
		bufferLayout.Add<DynamicConstantBuffer::DataType::Bool>("horizontal");
		bufferLayout.Add<DynamicConstantBuffer::DataType::Int>("strength");

		DynamicConstantBuffer::BufferData bufferData(bufferLayout);
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("horizontal") = horizontal;
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("strength") = 1;

		outlineScalingBuffer = std::make_shared<CachedBuffer>(gfx, bufferData, 0, true);
	}

	AddBindable(outlineScalingBuffer);
	AddBindable(PixelShader::GetBindable(gfx, "PS_Fullscreen_Outline.cso"));
	AddBindable(BlendState::GetBindable(gfx, false));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::MIRROR, 0, SamplerState::NEVER, SamplerState::POINT));
	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Mask));

	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("pixelShaderTexture", &m_renderTarget));
}


void OutlineScalingRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	m_renderTarget->Clear(gfx);

	FullscreenFilterRenderPass::Render(gfx);
}