#include "BufferClearRenderPass.h"
#include "Graphics.h"

BufferClearRenderPass::BufferClearRenderPass(GFX& gfx, const char* name)
	: RenderPass(name)
{
	RegisterOutput(RenderPassBufferOutput<GraphicBuffer>::GetUnique("buffer", &m_graphicBuffer));
	RegisterInput(RenderPassBufferInput<GraphicBuffer>::GetUnique("buffer", &m_graphicBuffer));
}

void BufferClearRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	m_graphicBuffer.get()->Clear(gfx);
}