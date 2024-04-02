#include "VerticalGaussBlurRenderPass.h"
#include "BindableClassesMacro.h"

VerticalGaussBlurRenderPass::VerticalGaussBlurRenderPass(class GFX& gfx, const char* name)
	: FullscreenFilterRenderPass(gfx, name)
{
	AddBindable(PixelShader::GetBindable(gfx, "PS_Fullscreen_GaussBlur.cso"));
	AddBindable(BlendState::GetBindable(gfx, true));
	AddBindable(SamplerState::GetBindable(gfx, true, true));
	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Mask));

	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));

	AddBindableInput<CachedBuffer>("gaussCooficientSettings");
	RegisterInput(RenderPassBindableInput<CachedBuffer>::GetUnique("gaussDirectionSettings", &gaussBlurDirectionData));
	AddBindableInput<RenderTargetWithTexture>("pixelShaderTexture");
	RegisterInput(RenderPassBufferInput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));
	RegisterInput(RenderPassBufferInput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));
}

void VerticalGaussBlurRenderPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	DynamicConstantBuffer::BufferData bufferData = gaussBlurDirectionData->GetBufferData();

	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("horizontal") = FALSE;

	gaussBlurDirectionData->Update(gfx, bufferData);

	gaussBlurDirectionData->Bind(gfx);

	FullscreenFilterRenderPass::Render(gfx);
}