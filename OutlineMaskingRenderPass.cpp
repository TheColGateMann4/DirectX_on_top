#include "OutlineMaskingRenderPass.h"
#include "BindableClassesMacro.h"

OutlineMaskingRenderPass::OutlineMaskingRenderPass(class GFX& gfx, const char* name)
	:	
	RenderJobPass(name)
{
	RegisterOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));

	RegisterInput(RenderPassBufferInput<DepthStencilView>::GetUnique("depthStencilView", &m_depthStencilView));

	AddBindable(NullShader::GetBindable(gfx, TargetPixelShader));
	AddBindable(VertexShader::GetBindable(gfx, "VS.cso"));

	AddBindable(DepthStencilState::GetBindable(gfx, DepthStencilState::StencilMode::Write));
	AddBindable(RasterizerState::GetBindable(gfx, false));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::CLAMP, 0, SamplerState::NEVER, SamplerState::POINT));
}