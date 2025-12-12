#pragma once
#include "wrl_no_warnings.h"
class GFX;

class GraphicBuffer
{
public:
	virtual ~GraphicBuffer() = default;

public:
	virtual void BindRenderTarget(GFX& gfx, GraphicBuffer* graphicBuffer = nullptr) = 0;

	void SaveToFile(GFX& gfx);

private:
	virtual void GetBuffer(struct ID3D11Resource** resource) = 0;

public:
	virtual void Clear(GFX& gfx) const = 0;

private:
	static size_t m_savedImages;
};

