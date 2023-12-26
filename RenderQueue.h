#pragma once
#include <vector>
#include "RenderPass.h"
#include "RenderJob.h"

class RenderQueue
{
public:
	void Render(GFX& gfx);

public:
	void AddRenderJob(const RenderJob& job, PASS_TYPE passType);

private:
	std::vector<RenderPass> m_passes = {};
};

