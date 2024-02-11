#include "RenderQueue.h"
#include "Graphics.h"
#include "BindableClassesMacro.h"

RenderQueue::RenderQueue(GFX& gfx)
	:
	m_depthStencilView(gfx),
	m_renderTargetFirst(gfx, 1),
	m_renderTargetSecond(gfx, 1),
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
	bool isGaussFilter = fullscreenfilter.currentShaderName == "GaussBlur";

	m_renderTargetFirst.ClearBuffer(gfx);
	m_renderTargetSecond.ClearBuffer(gfx);
	m_depthStencilView.Clear(gfx);

	gfx.BindRenderTarget(m_depthStencilView);

	BlendState::GetBindable(gfx, false)->Bind(gfx);
	SamplerState::GetBindable(gfx, true, false)->Bind(gfx);
	DepthStencilState::GetBindable(gfx, DepthStencilState::Off)->Bind(gfx);
	m_passes.at(PASS_NORMAL).Execute(gfx);

	SamplerState::GetBindable(gfx, false, false)->Bind(gfx);
	NullPixelShader::GetBindable(gfx)->Bind(gfx);
	DepthStencilState::GetBindable(gfx, DepthStencilState::Write)->Bind(gfx);
	m_passes.at(PASS_WRITE).Execute(gfx);

	if (isGaussFilter)
	{
		m_renderTargetFirst.ChangeDownscalingRatio(fullscreenfilter.GetGuassBlurFilter()->GetDownscalingRatio());
		m_renderTargetSecond.ChangeDownscalingRatio(fullscreenfilter.GetGuassBlurFilter()->GetDownscalingRatio());
	}
	else
		m_renderTargetFirst.ChangeDownscalingRatio(1);

	m_renderTargetFirst.BindRenderTarget(gfx);
	
	DepthStencilState::GetBindable(gfx, DepthStencilState::Off)->Bind(gfx);
	m_passes.at(PASS_MASK).Execute(gfx);

	if (isGaussFilter)
		m_renderTargetSecond.BindRenderTarget(gfx);
	else
		gfx.BindRenderTarget(m_depthStencilView);


	fullscreenfilter.Bind(gfx);
	SamplerState::GetBindable(gfx, true, true)->Bind(gfx);

	if (isGaussFilter)
	{
		m_renderTargetFirst.BindTexture(gfx, 0);
		fullscreenfilter.GetGuassBlurFilter()->Bind(gfx);
		fullscreenfilter.GetGuassBlurFilter()->SetHorizontal(gfx, true);
	}
	else
	{
		m_renderTargetFirst.BindTexture(gfx, 0);
		DepthStencilState::GetBindable(gfx, DepthStencilState::Mask)->Bind(gfx);
		BlendState::GetBindable(gfx, true)->Bind(gfx);
		fullscreenfilter.m_constBuffer->Bind(gfx);
	}

	fullscreenfilter.Draw(gfx);

	if (isGaussFilter)
	{
		gfx.BindRenderTarget(m_depthStencilView);
		m_renderTargetSecond.BindTexture(gfx, 0);
		fullscreenfilter.GetGuassBlurFilter()->SetHorizontal(gfx, false);
		BlendState::GetBindable(gfx, true)->Bind(gfx);
		DepthStencilState::GetBindable(gfx, DepthStencilState::Mask)->Bind(gfx);

		fullscreenfilter.Draw(gfx);
	}
}

void RenderQueue::AddRenderJob(const RenderJob& job, PASS_TYPE passType)
{
	if (!m_passes_initialized)
	{
		m_passes.resize(3);
		m_passes_initialized = true;
	}

	m_passes.at(passType).AddRenderJob(job);
}