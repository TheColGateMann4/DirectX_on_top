#pragma once
#include "SamplerState.h"

SamplerState::SamplerState(GFX& gfx, D3D11_TEXTURE_ADDRESS_MODE mode)
	: m_mode(mode)
{
	HRESULT hr;

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

	THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSamplerState))
}

void SamplerState::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());
}