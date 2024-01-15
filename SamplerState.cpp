#pragma once
#include "SamplerState.h"

SamplerState::SamplerState(GFX& gfx, bool anisotropicFiltering, bool mirror)
	: m_anisotropicFiltering(anisotropicFiltering),
	m_mirror(mirror)
{
	HRESULT hr;

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
	samplerDesc.Filter = m_anisotropicFiltering ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = m_mirror ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = m_mirror ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

	THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSamplerState))
}

void SamplerState::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());
}