#include "RenderStep.h"
#include "RenderGraph.h"
#include "RenderJob.h"
#include "RenderJobPass.h"

RenderStep::RenderStep(const RenderStep& renderStep) noexcept
{
	m_pass = renderStep.m_pass;

	m_bindables.reserve(renderStep.m_bindables.size());

	for (auto& bindable : renderStep.m_bindables)
		m_bindables.push_back(bindable);

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