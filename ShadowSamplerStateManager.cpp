#include "ShadowSamplerStateManager.h"
#include "Graphics.h"

ShadowSamplerStateManager::ShadowSamplerStateManager(GFX& gfx)
{
	m_samplerComparisonStates.push_back(SamplerState::GetBindable(gfx, SamplerState::BORDER, 2, SamplerState::LESS_EQUAL, SamplerState::POINT));
	m_samplerComparisonStates.push_back(SamplerState::GetBindable(gfx, SamplerState::BORDER, 2, SamplerState::LESS_EQUAL, SamplerState::BILINEAR));

	m_samplerStates.push_back(SamplerState::GetBindable(gfx, SamplerState::BORDER, 1, SamplerState::NEVER, SamplerState::POINT));
	m_samplerStates.push_back(SamplerState::GetBindable(gfx, SamplerState::BORDER, 1, SamplerState::NEVER, SamplerState::BILINEAR));

	m_samplerComparisonStates.at(0)->Bind(gfx);
	m_samplerStates.at(0)->Bind(gfx);
}

void ShadowSamplerStateManager::Bind(GFX& gfx) const
{
	if (m_comparisonSampler)
		m_samplerComparisonStates.at(static_cast<int>(m_bilinear))->Bind(gfx);
	else
		m_samplerStates.at(static_cast<int>(m_bilinear))->Bind(gfx);
}

void ShadowSamplerStateManager::SetActiveSampler(bool comparisonSampler, bool bilinear)
{
	m_comparisonSampler = comparisonSampler;
	m_bilinear = bilinear;
}