#include "RenderTechnique.h"


void RenderTechnique::Render(const Shape* shape) const noexcept
{
	for (const auto& step : m_steps)
		step->Render(shape);
}

void RenderTechnique::LinkToPipeline(class RenderGraph& renderGraph)
{
	for (auto& step : m_steps)
		step->LinkToPipeline(renderGraph);
}