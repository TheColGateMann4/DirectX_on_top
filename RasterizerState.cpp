#include "RasterizerState.h"
#include "ErrorMacros.h"

RasterizerState::RasterizerState(GFX& gfx, bool disableBackfaceCulling)
{
	m_disableBackfaceCulling = disableBackfaceCulling;

	HRESULT hr;

	D3D11_RASTERIZER_DESC rasterizerDesc = { CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{}) };
	rasterizerDesc.CullMode = disableBackfaceCulling ? D3D11_CULL_NONE : D3D11_CULL_BACK;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateRasterizerState(&rasterizerDesc, &pResterizerState));
}

void RasterizerState::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->RSSetState(pResterizerState.Get());
}