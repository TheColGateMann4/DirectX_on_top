#include "HorizontalGaussBlurRenderPass.h"
#include "BindableClassesMacro.h"

HorizontalGaussBlurRenderPass::HorizontalGaussBlurRenderPass(class GFX& gfx, const char* name, const int width, const int height)
	: FullscreenFilterRenderPass(gfx, name)
{
	m_renderTarget = std::make_shared<RenderTargetWithTexture>(gfx, width / 2, height / 2, 0);

	RegisterInput(RenderPassBufferInput<RenderTarget>::GetUnique("pixelShaderTexture", &m_renderTarget));
	
	AddBindableOutput<CachedBuffer>("gaussCooficientSettings");
	RegisterOutput(RenderPassBindableOutput<CachedBuffer>::GetUnique("gaussDirectionSettings", gaussBlurDirectionData.get()));
	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("pixelShaderTexture", &m_renderTarget));
	RegisterOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));

	AddBindable(PixelShader::GetBindable(gfx, "PS_Fullscreen_GaussBlur.cso"));
	AddBindable(BlendState::GetBindable(gfx, true));
	AddBindable(SamplerState::GetBindable(gfx, true, true));
}

void HorizontalGaussBlurRenderPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	DynamicConstantBuffer::BufferData bufferData = gaussBlurDirectionData->GetBufferData();

	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("horizontal") = TRUE;

	gaussBlurDirectionData->Update(gfx, bufferData);

	gaussBlurDirectionData->Bind(gfx);

	FullscreenFilterRenderPass::Render(gfx);
}