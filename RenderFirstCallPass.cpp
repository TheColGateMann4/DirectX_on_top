#include "RenderFirstCallPass.h"

void RenderFirstCallPass::FrameStarted() noexcept
{
	m_called = false;
}


RenderFirstCallJobPass::RenderFirstCallJobPass(const char* jobPassName)
	:
	RenderJobPass(jobPassName)
{

}

void RenderFirstCallJobPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	if (!m_called)
		this->FirstRenderCall(gfx);
	else
		this->FurtherRenderCall(gfx);


	m_called = true;
}