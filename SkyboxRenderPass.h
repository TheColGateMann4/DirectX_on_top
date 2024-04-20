#pragma once
#include "RenderJobPass.h"
#include "ConstantBuffers.h"
#include "Includes.h"

class SkyboxRenderPass : public RenderJobPass
{
public:
	SkyboxRenderPass(class GFX& gfx, const char* name);
};

