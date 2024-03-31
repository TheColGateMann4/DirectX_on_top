#pragma once
class GFX;

class GraphicBuffer
{
public:
	virtual ~GraphicBuffer() = default;

public:
	virtual void BindRenderTarget(GFX& gfx, GraphicBuffer* graphicBuffer = nullptr) = 0;

public:
	virtual void Clear(GFX& gfx) const = 0;
};

