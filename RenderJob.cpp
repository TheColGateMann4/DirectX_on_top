#include "RenderJob.h"
#include "RenderStep.h"
#include "Shape.h"

RenderJob::RenderJob(const Shape* shape, const RenderStep* step)
	: m_shape(shape), m_step(step)
{

}

void RenderJob::Bind(GFX& gfx) const
{
	if (!m_step->m_active || !m_shape->m_visible)
		return;

	m_shape->Bind(gfx);
	m_step->Bind(gfx);

	gfx.DrawIndexed(m_shape->GetIndexCount());

	m_step->PostRenderBind(gfx);
}