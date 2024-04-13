#pragma once
#include "SamplerState.h"
#include "ErrorMacros.h"
#include "Graphics.h"

SamplerState::SamplerState(GFX& gfx, bool anisotropicFiltering, Mode samplerMode, size_t slot)
	: 
	m_anisotropicFiltering(anisotropicFiltering),
	m_samplerMode(samplerMode),
	m_slot(slot)
{
	HRESULT hr;

	D3D11_TEXTURE_ADDRESS_MODE dxSamplerMode = *reinterpret_cast<D3D11_TEXTURE_ADDRESS_MODE*>(&m_samplerMode);

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
	samplerDesc.Filter = m_anisotropicFiltering ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = dxSamplerMode;
	samplerDesc.AddressV = dxSamplerMode;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	samplerDesc.BorderColor[0] = 0.0f;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSamplerState))
}

void SamplerState::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->PSSetSamplers(m_slot, 1, pSamplerState.GetAddressOf());
}