#include "BlendState.h"
#include "ErrorMacros.h"

BlendState::BlendState(GFX& gfx, bool blend)
{
	m_blend = blend;
	HRESULT hr;

	D3D11_BLEND_DESC blendDesc = {};

	auto& renderTargetBlendDesc = blendDesc.RenderTarget[0];

	renderTargetBlendDesc.BlendEnable = m_blend;

	if (m_blend)
	{
		renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		renderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
		renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else
	{
		renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}


	THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlendState));
}

void BlendState::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->OMSetBlendState(pBlendState.Get(), NULL, UINT32_MAX);
}