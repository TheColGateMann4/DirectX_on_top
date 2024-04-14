#pragma once
#include "Includes.h"
#include "SamplerState.h"

class ShadowSamplerStateManager
{
public:
	ShadowSamplerStateManager(class GFX& gfx);

public:
	void Bind(GFX& gfx) const;

	void SetActiveSampler(bool comparisonSampler, bool bilinear);

private:
	std::vector<std::shared_ptr<SamplerState>> m_samplerComparisonStates = {};
	std::vector<std::shared_ptr<SamplerState>> m_samplerStates = {};

	bool m_comparisonSampler = false;
	bool m_bilinear = true;
};

