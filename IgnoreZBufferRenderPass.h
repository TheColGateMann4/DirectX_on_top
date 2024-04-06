#pragma once
#include "RenderJobPass.h"

class IgnoreZBufferRenderPass : public RenderJobPass
{
public:
	IgnoreZBufferRenderPass(class GFX& gfx, const char* name);
};
