#include "BufferClearRenderPass.h"
#include "Graphics.h"

BufferClearRenderPass::BufferClearRenderPass(GFX& gfx, const char* name)
	: RenderPass(name)
{
	RegisterInput(RenderPassBufferInput<GraphicBuffer>::GetUnique("buffer", &m_graphicBuffer));
	RegisterOutput(RenderPassBufferOutput<GraphicBuffer>::GetUnique("buffer", &m_graphicBuffer));
}

void BufferClearRenderPass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	m_graphicBuffer.get()->Clear(gfx);
}