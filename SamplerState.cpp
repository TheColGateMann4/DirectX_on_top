#pragma once
#include "SamplerState.h"

SamplerState::SamplerState(GFX& gfx, D3D11_TEXTURE_ADDRESS_MODE mode)
{
	HRESULT hr;

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY; // x16
	samplerDesc.AddressU = mode;
	samplerDesc.AddressV = mode;
	samplerDesc.AddressW = mode;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSamplerState))
}

void SamplerState::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());
}