#include "BoxBlurRenderPass.h"
#include "RenderTarget.h"
#include "BindableClassesMacro.h"

BoxBlurRenderPass::BoxBlurRenderPass(class GFX& gfx, const char* name)
	: FullscreenFilterRenderPass(gfx, name)
{
	{
		DynamicConstantBuffer::BufferLayout constBufferLayout;
		constBufferLayout.Add<DynamicConstantBuffer::DataType::Int>("strength");

		DynamicConstantBuffer::BufferData constBufferData(constBufferLayout);
		*constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("strength") = 3;

		blurData = std::make_shared<CachedBuffer>(gfx, constBufferData, 0, true);
	}

	RegisterInput(RenderPassBufferInput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));

	AddBindableOutput<RenderTargetWithTexture>("pixelShaderTexture");
	RegisterOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("renderTarget", &m_renderTarget));
	RegisterOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));

	AddBindable(PixelShader::GetBindable(gfx, "PS_Fullscreen_BoxBlur.cso"));
	AddBindable(SamplerState::GetBindable(gfx, true, true));
	AddBindable(BlendState::GetBindable(gfx, true));
	AddBindable(blurData);
}