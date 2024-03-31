#include "RenderJobPass.h"
#include "RenderJob.h"
#include "Graphics.h"

void RenderJobPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	RenderBindablePass::Render(gfx);

	for (const auto& job : m_jobs)
		job.Bind(gfx);
}

void RenderJobPass::AddRenderJob(const RenderJob& job)
{
	m_jobs.push_back(job);
}

void RenderJobPass::Reset()
{
	m_jobs.clear();
}