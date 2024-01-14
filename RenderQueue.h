#pragma once
#include <vector>
#include "RenderPass.h"
#include "RenderJob.h"
#include "DepthStencilView.h"
#include "RenderTarget.h"
#include "FullscreenFilter.h"

class RenderQueue
{
public:
	RenderQueue(GFX& gfx);

public:
	void Render(GFX& gfx);

public:
	void AddRenderJob(const RenderJob& job, PASS_TYPE passType);

private:
	std::vector<RenderPass> m_passes = {};
	DepthStencilView m_depthStencilView;
	RenderTarget m_renderTarget;
	FullscreenFilter m_fullscreenfilter;
};

