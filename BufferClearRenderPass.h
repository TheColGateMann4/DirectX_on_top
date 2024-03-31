#pragma once
#include "RenderPass.h"
#include "GraphicBuffer.h"

class BufferClearRenderPass : public RenderPass
{
public:
	BufferClearRenderPass(class GFX& gfx, const char* name);

public:
	virtual void Render(class GFX& gfx) const noexcept(!_DEBUG) override;
	
private:
	std::shared_ptr<GraphicBuffer> m_graphicBuffer;
};

