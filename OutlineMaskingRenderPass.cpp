#include "OutlineMaskingRenderPass.h"
#include "BindableClassesMacro.h"

OutlineMaskingRenderPass::OutlineMaskingRenderPass(class GFX& gfx, const char* name)
	:	
	RenderJobPass(name)
{
	RegisterOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));

	RegisterInput(RenderPassBufferInput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));

	AddBindable(NullPixelShader::GetBindable(gfx));
	AddBindable(VertexShader::GetBindable(gfx, "VS.cso"));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Write));
	AddBindable(RasterizerState::GetBindable(gfx, false));
	AddBindable(SamplerState::GetBindable(gfx, false, SamplerState::Mode::CLAMP));
}