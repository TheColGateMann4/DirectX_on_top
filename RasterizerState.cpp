#include "RasterizerState.h"

RasterizerState::RasterizerState(GFX& gfx, bool disableBackfaceCulling)
{
	m_disableBackfaceCulling = disableBackfaceCulling;

	HRESULT hr;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = disableBackfaceCulling ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
	rasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;

	THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateRasterizerState(&rasterizerDesc, &pResterizerState));
}

void RasterizerState::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->RSSetState(pResterizerState.Get());
}