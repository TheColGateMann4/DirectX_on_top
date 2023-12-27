#include "RenderJob.h"
#include "RenderSteps.h"
#include "Shape.h"

RenderJob::RenderJob(const Shape* shape, const RenderSteps* technique)
	: m_shape(shape), m_step(technique)
{

}

void RenderJob::Bind(GFX& gfx) const
{
	if (!m_step->m_active)
		return;

	m_shape->Bind(gfx);
	m_step->Bind(gfx);

	gfx.DrawIndexed(m_shape->GetIndexCount());
}