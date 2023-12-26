#include "RenderSteps.h"
#include "RenderQueue.h"

void RenderSteps::Execute(RenderQueue& renderQueue, const Shape* shape) const noexcept
{
	renderQueue.AddRenderJob(RenderJob(shape, this), m_stepType);
}

void RenderSteps::Bind(GFX& gfx) const noexcept
{
	for (const auto& bindable : m_bindables)
		bindable->Bind(gfx);
}

void RenderSteps::AddBindable(std::shared_ptr<Bindable> bindable)
{
	m_bindables.push_back(bindable);
}