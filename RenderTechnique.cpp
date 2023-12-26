#include "RenderTechnique.h"


void RenderTechnique::Execute(RenderQueue& renderQueue, const Shape* shape) const noexcept
{
	for (const auto& step : m_steps)
		step.Execute(renderQueue, shape);
}