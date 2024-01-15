#include "RenderQueue.h"
#include "Graphics.h"
#include "BindableClassesMacro.h"

RenderQueue::RenderQueue(GFX& gfx)
	:
	m_depthStencilView(gfx),
	m_renderTarget(gfx),
	m_fullscreenfilter(gfx)
{}

void RenderQueue::ChangeScreenFilter(GFX& gfx, std::string ShaderName)
{
	m_fullscreenfilter.ChangePixelShader(gfx, ShaderName);
}

void RenderQueue::ChangeBlurStrength(GFX& gfx, int strength)
{
	m_fullscreenfilter.ChangeBlurStrength(gfx, strength);
}

void RenderQueue::Render(GFX& gfx)
{
	m_renderTarget.ClearBuffer(gfx, DirectX::XMFLOAT4{ 0.0f, 0.0f, 0.0f, 0.0f });
	m_depthStencilView.Clear(gfx);
	gfx.BindSwapBuffer(m_depthStencilView);
	BlendState::GetBindable(gfx, false)->Bind(gfx);
	SamplerState::GetBindable(gfx, true, false)->Bind(gfx);
	DepthStencilState::GetBindable(gfx, DepthStencilState::Off)->Bind(gfx);

	m_passes.at(PASS_NORMAL).Execute(gfx);


	NullPixelShader::GetBindable(gfx)->Bind(gfx);
	DepthStencilState::GetBindable(gfx, DepthStencilState::Write)->Bind(gfx);

	m_passes.at(PASS_WRITE).Execute(gfx);

	m_renderTarget.BindRenderTarget(gfx);
	DepthStencilState::GetBindable(gfx, DepthStencilState::Mask)->Bind(gfx);

	m_passes.at(PASS_MASK).Execute(gfx);

	gfx.BindSwapBuffer(m_depthStencilView);
	m_renderTarget.BindTexture(gfx, 0);
	m_fullscreenfilter.Render(gfx);
}

void RenderQueue::AddRenderJob(const RenderJob& job, PASS_TYPE passType)
{
	if (m_passes.size() != 3)
		m_passes.resize(3);

	m_passes.at(passType).AddRenderJob(job);
}