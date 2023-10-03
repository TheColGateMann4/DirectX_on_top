#include "BlendState.h"

BlendState::BlendState(GFX& gfx, bool blend)
{
	HRESULT hr;

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = blend;

	if (blend)
	{
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else
	{
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}


	THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlendState));
}

void BlendState::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->OMSetBlendState(pBlendState.Get(), NULL, UINT32_MAX);
}