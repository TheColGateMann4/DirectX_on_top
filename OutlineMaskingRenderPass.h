#pragma once
#include "RenderJobPass.h"

class OutlineMaskingRenderPass : public RenderJobPass
{
public:
	OutlineMaskingRenderPass(class GFX& gfx, const char* name);
};

