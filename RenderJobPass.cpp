#include "RenderJobPass.h"
#include "RenderJob.h"
#include "Graphics.h"

void RenderJobPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	RenderBindablePass::Render(gfx);

	for (const auto& job : m_jobs)
		job.Bind(gfx);

	if(m_captureFrame)
	{
		if (m_renderTarget)
			m_renderTarget->SaveToFile(gfx);

		if (m_depthStencilView)
			m_depthStencilView->SaveToFile(gfx);

		m_captureFrame = false;
	}
}

void RenderJobPass::AddRenderJob(const RenderJob& job)
{
	m_jobs.push_back(job);
}

void RenderJobPass::Reset()
{
	m_jobs.clear();
}

void RenderJobPass::CaptureNextFrame()
{
	m_captureFrame = true;
}