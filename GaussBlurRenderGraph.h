#pragma once
#include "RenderGraph.h"

class GaussBlurRenderGraph : public RenderGraph
{
public:
	GaussBlurRenderGraph(class GFX& gfx);

	void SetCooficients(class GFX& gfx, int range, float sigma, std::shared_ptr<CachedBuffer>* cooficientSettings) noexcept;

private:
	float CalculateGaussDensity(const float x, const float sigma) const noexcept;

private:
	std::shared_ptr<CachedBuffer> cooficientSettings;
	std::shared_ptr<CachedBuffer> directionSettings;
};