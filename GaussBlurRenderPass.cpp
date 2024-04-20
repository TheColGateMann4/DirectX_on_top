#include "GaussBlurRenderPass.h"
#include "BindableClassesMacro.h"

VerticalGaussBlurRenderPass::VerticalGaussBlurRenderPass(class GFX& gfx, const char* name)
	: FullscreenFilterRenderPass(gfx, name)
{
	AddBindable(PixelShader::GetBindable(gfx, "PS_Fullscreen_GaussBlur.cso"));
	AddBindable(BlendState::GetBindable(gfx, true));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::MIRROR, 0, SamplerState::NEVER, SamplerState::POINT));
	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Mask));

	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));
	RegisterOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));

	AddBindableInput<CachedBuffer>("gaussCooficientSettings");
	RegisterInput(RenderPassBindableInput<CachedBuffer>::GetUnique("gaussDirectionSettings", &gaussBlurDirectionData));
	AddBindableInput<RenderTargetWithTexture>("pixelShaderTexture");
	RegisterInput(RenderPassBufferInput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));
	RegisterInput(RenderPassBufferInput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));
}

void VerticalGaussBlurRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	DynamicConstantBuffer::BufferData bufferData = gaussBlurDirectionData->GetBufferData();

	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("horizontal") = FALSE;

	gaussBlurDirectionData->Update(gfx, bufferData);

	gaussBlurDirectionData->Bind(gfx);

	FullscreenFilterRenderPass::Render(gfx);
}



HorizontalGaussBlurRenderPass::HorizontalGaussBlurRenderPass(class GFX& gfx, const char* name, const int width, const int height)
	: FullscreenFilterRenderPass(gfx, name)
{
	m_renderTarget = std::make_shared<RenderTargetWithTexture>(gfx, width / 2, height / 2, 0);

	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("pixelShaderTexture", &m_renderTarget));

	AddBindableInput<CachedBuffer>("gaussCooficientSettings");
	AddBindableInput<RenderTargetWithTexture>("pixelShaderTexture");
	RegisterInput(RenderPassBindableInput<CachedBuffer>::GetUnique("gaussDirectionSettings", &gaussBlurDirectionData));

	AddBindable(PixelShader::GetBindable(gfx, "PS_Fullscreen_GaussBlur.cso"));
	AddBindable(BlendState::GetBindable(gfx, false));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::MIRROR, 0, SamplerState::NEVER, SamplerState::ANISOTROPIC));
}

void HorizontalGaussBlurRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	DynamicConstantBuffer::BufferData bufferData = gaussBlurDirectionData->GetBufferData();

	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("horizontal") = TRUE;

	gaussBlurDirectionData->Update(gfx, bufferData);

	gaussBlurDirectionData->Bind(gfx);

	FullscreenFilterRenderPass::Render(gfx);
}