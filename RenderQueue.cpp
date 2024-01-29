#include "RenderQueue.h"
#include "Graphics.h"
#include "BindableClassesMacro.h"

RenderQueue::RenderQueue(GFX& gfx)
	:
	m_depthStencilView(gfx),
	m_renderTargetHorizontal(gfx),
	m_renderTargetVertical(gfx),
	fullscreenfilter(gfx)
{}

void RenderQueue::ChangeScreenFilter(GFX& gfx, std::string ShaderName)
{
	fullscreenfilter.ChangePixelShader(gfx, ShaderName);
}

void RenderQueue::ChangeBlurStrength(GFX& gfx, int strength)
{
	fullscreenfilter.ChangeBlurStrength(gfx, strength);
}

void RenderQueue::Render(GFX& gfx)
{
	if (fullscreenfilter.currentShaderName == "GaussBlur")
	{
		m_depthStencilView.Clear(gfx);
		m_renderTargetVertical.ClearBuffer(gfx);
		m_renderTargetHorizontal.ClearBuffer(gfx);
		m_renderTargetHorizontal.BindRenderTarget(gfx, m_depthStencilView);

		BlendState::GetBindable(gfx, false)->Bind(gfx);
		SamplerState::GetBindable(gfx, true, false)->Bind(gfx);
		DepthStencilState::GetBindable(gfx, DepthStencilState::Off)->Bind(gfx);

		m_passes.at(PASS_NORMAL).Execute(gfx);

		m_renderTargetVertical.BindRenderTarget(gfx);
		m_renderTargetHorizontal.BindTexture(gfx, 0);

		fullscreenfilter.Bind(gfx);
		fullscreenfilter.BindGaussBlur(gfx);
		fullscreenfilter.GetGuassBlurFilter()->SetHorizontal(gfx, true);
		fullscreenfilter.Draw(gfx);

		gfx.BindRenderTarget();
		m_renderTargetVertical.BindTexture(gfx, 0);
		fullscreenfilter.GetGuassBlurFilter()->SetHorizontal(gfx, false);
		fullscreenfilter.Draw(gfx);
	}
	else
	{
		m_renderTargetHorizontal.ClearBuffer(gfx, DirectX::XMFLOAT4{ 0.0f, 0.0f, 0.0f, 0.0f });
		m_depthStencilView.Clear(gfx);
		m_renderTargetHorizontal.BindRenderTarget(gfx, m_depthStencilView);

		gfx.BindRenderTarget(m_depthStencilView);
		BlendState::GetBindable(gfx, false)->Bind(gfx);
		SamplerState::GetBindable(gfx, true, false)->Bind(gfx);
		DepthStencilState::GetBindable(gfx, DepthStencilState::Off)->Bind(gfx);

		m_passes.at(PASS_NORMAL).Execute(gfx);
		NullPixelShader::GetBindable(gfx)->Bind(gfx);
		DepthStencilState::GetBindable(gfx, DepthStencilState::Write)->Bind(gfx);

		m_passes.at(PASS_WRITE).Execute(gfx);

		m_renderTargetHorizontal.BindRenderTarget(gfx);
		DepthStencilState::GetBindable(gfx, DepthStencilState::Mask)->Bind(gfx);

		m_passes.at(PASS_MASK).Execute(gfx);

		SamplerState::GetBindable(gfx, false, true)->Bind(gfx);
		gfx.BindRenderTarget(m_depthStencilView);
		m_renderTargetHorizontal.BindTexture(gfx, 0);
		fullscreenfilter.Bind(gfx);
		fullscreenfilter.Draw(gfx);
	}
}

void RenderQueue::AddRenderJob(const RenderJob& job, PASS_TYPE passType)
{
	if (m_passes.size() != 3)
		m_passes.resize(3);

	m_passes.at(passType).AddRenderJob(job);
}