#pragma once
#include <memory>

class GaussBlurFilter
{
public:
	GaussBlurFilter(class GFX& gfx, int range = 5, float sigma = 1.0f);

public:
	void Bind(class GFX& gfx) const noexcept;

public:
	void SetCooficients(class GFX& gfx, int range, float sigma) noexcept;
	void SetHorizontal(class GFX& gfx, bool horizontal);
	float CalculateGaussDensity(const float range, const float sigma) const noexcept;

public:
	int GetDownscalingRatio() const noexcept;

public:
	void MakeImGuiPropeties(GFX& gfx);

private:
	int m_range;
	float m_sigma;
	int m_downscalling = 1;

	std::shared_ptr<class CachedBuffer> m_cooficients = nullptr;
	std::shared_ptr<class CachedBuffer> m_blurSettings = nullptr;
};

