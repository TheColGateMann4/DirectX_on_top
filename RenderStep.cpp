#include "RenderStep.h"
#include "RenderGraph.h"
#include "RenderJob.h"
#include "RenderJobPass.h"

RenderStep::RenderStep(const RenderStep& renderStep) noexcept
{
	m_pass = renderStep.m_pass;

	{
		m_bindables.reserve(renderStep.m_bindables.size());

		for (auto& bindable : renderStep.m_bindables)
			m_bindables.push_back(bindable);
	}

	{
		m_postRenderBindables.reserve(renderStep.m_postRenderBindables.size());

		for (auto& bindable : renderStep.m_postRenderBindables)
			m_postRenderBindables.push_back(bindable);
	}

	m_targetPassName = renderStep.m_targetPassName;
	m_active = renderStep.m_active;
	m_linked = renderStep.m_linked;
}

void RenderStep::Render(const Shape* shape) const noexcept
{
	if (m_active)
	{
		assert(m_linked);
		m_pass->AddRenderJob(RenderJob(shape, this));
	}
}

void RenderStep::Bind(GFX& gfx) const noexcept
{
	if(m_active)
		for (const auto& bindable : m_bindables)
			bindable->Bind(gfx);
}

void RenderStep::PostRenderBind(GFX& gfx) const noexcept
{
	if (m_active)
		for (const auto& bindable : m_postRenderBindables)
			bindable->Bind(gfx);
}

void RenderStep::LinkToPipeline(RenderGraph& renderGraph)
{
	if (!m_linked)
	{
		m_linked = true;

		m_pass = renderGraph.GetRenderJobPass(m_targetPassName);
	}
}

void RenderStep::AddBindable(std::shared_ptr<Bindable> bindable)
{
	m_bindables.push_back(bindable);
}

void RenderStep::AddPostRenderBindable(std::shared_ptr<Bindable> bindable)
{
	m_postRenderBindables.push_back(bindable);
}



void TempRenderStep::Render(const Shape* shape) const noexcept
{
	if (m_active)
	{
		assert(m_linked);
		m_pass->AddTempRenderJob(RenderJob(shape, this));
	}
}