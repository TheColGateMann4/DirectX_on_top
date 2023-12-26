#include "RenderQueue.h"
#include "Graphics.h"
#include "BindableClassesMacro.h"

void RenderQueue::Render(GFX& gfx)
{
	DepthStencil::GetBindable(gfx, DepthStencil::Off)->Bind(gfx);

	m_passes.at(PASS_NORMAL).Execute(gfx);


	NullPixelShader::GetBindable(gfx)->Bind(gfx);
	DepthStencil::GetBindable(gfx, DepthStencil::Write)->Bind(gfx);

	m_passes.at(PASS_WRITE).Execute(gfx);


	DepthStencil::GetBindable(gfx, DepthStencil::Mask)->Bind(gfx);

	m_passes.at(PASS_MASK).Execute(gfx);
}

void RenderQueue::AddRenderJob(const RenderJob& job, PASS_TYPE passType)
{
	if (m_passes.size() != 3)
		m_passes.resize(3);

	m_passes.at(passType).AddRenderJob(job);
}