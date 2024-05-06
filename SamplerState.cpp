#pragma once
#include "SamplerState.h"
#include "ErrorMacros.h"
#include "Graphics.h"

SamplerState::SamplerState(GFX& gfx, Mode samplerMode, size_t slot, Comparison comparison, Filter filter, bool isPixelShaderResource)
	: 
	m_samplerMode(samplerMode),
	m_slot(slot),
	m_comparison(comparison),
	m_filter(filter),
	m_isPixelShaderResource(isPixelShaderResource)
{
	HRESULT hr;
	
	// If we use any comparison mode, then it means that we want comparison sampler state, so we use right filter for that.
	// In D3D11_FILTER struct, 0x80 starts comparison filter options and they are equivalents of those with substracted 0x80
	size_t dxCorespondingFilterNumber = *reinterpret_cast<size_t*>(&m_filter) + ((comparison != NEVER) ? 0x80 : 0x0);

	D3D11_TEXTURE_ADDRESS_MODE dxSamplerMode = *reinterpret_cast<D3D11_TEXTURE_ADDRESS_MODE*>(&m_samplerMode);
	D3D11_COMPARISON_FUNC dxComparisonFunc = *reinterpret_cast<D3D11_COMPARISON_FUNC*>(&m_comparison);
	D3D11_FILTER dxFilter = *reinterpret_cast<D3D11_FILTER*>(&dxCorespondingFilterNumber);

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
	samplerDesc.Filter = dxFilter;
	samplerDesc.AddressU = dxSamplerMode;
	samplerDesc.AddressV = dxSamplerMode;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	samplerDesc.ComparisonFunc = dxComparisonFunc;
	samplerDesc.BorderColor[0] = 0.0f;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSamplerState))
}

void SamplerState::Bind(GFX& gfx) noexcept
{
	if(m_isPixelShaderResource)
		GFX::GetDeviceContext(gfx)->PSSetSamplers(m_slot, 1, pSamplerState.GetAddressOf());
	else
		GFX::GetDeviceContext(gfx)->VSSetSamplers(m_slot, 1, pSamplerState.GetAddressOf());

}