#include "RasterizerState.h"
#include "ErrorMacros.h"

RasterizerState::RasterizerState(GFX& gfx, bool disableBackfaceCulling, INT depthBias, FLOAT depthBiasClamp, FLOAT slopeScaledDepthBias)
	:
	m_disableBackfaceCulling(disableBackfaceCulling),
	m_depthBias(depthBias),
	m_depthBiasClamp(depthBiasClamp),
	m_slopeScaledDepthBias(slopeScaledDepthBias)
{
	HRESULT hr;

	D3D11_RASTERIZER_DESC rasterizerDesc = { CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{}) };
	rasterizerDesc.CullMode = m_disableBackfaceCulling ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = m_depthBias;
	rasterizerDesc.DepthBiasClamp = m_depthBiasClamp;
	rasterizerDesc.SlopeScaledDepthBias = m_slopeScaledDepthBias;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateRasterizerState(&rasterizerDesc, &pResterizerState));
}

void RasterizerState::ChangeDepthValues(GFX& gfx, INT depthBias, FLOAT depthBiasClamp, FLOAT slopeScaledDepthBias)
{
	m_depthBias = depthBias;
	m_depthBiasClamp = depthBiasClamp;
	m_slopeScaledDepthBias = slopeScaledDepthBias;
	
	HRESULT hr;

	D3D11_RASTERIZER_DESC rasterizerDesc = { CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{}) };
	rasterizerDesc.CullMode = m_disableBackfaceCulling ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = m_depthBias;
	rasterizerDesc.DepthBiasClamp = m_depthBiasClamp;
	rasterizerDesc.SlopeScaledDepthBias = m_slopeScaledDepthBias;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateRasterizerState(&rasterizerDesc, &pResterizerState));
}

void RasterizerState::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->RSSetState(pResterizerState.Get());
}