#include "DepthStencilView.h"
#include "Graphics.h"
#include "Includes.h"
#include "ErrorMacros.h"
#include <wrl.h>

DepthStencilView::DepthStencilView(GFX& gfx)
{
	HRESULT hr;


	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;

	D3D11_TEXTURE2D_DESC depthDecs = {};
	depthDecs.Width = gfx.GetWidth();
	depthDecs.Height = gfx.GetHeight();
	depthDecs.MipLevels = 1;
	depthDecs.ArraySize = 1;
	depthDecs.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDecs.SampleDesc.Count = 1;
	depthDecs.SampleDesc.Quality = 0;
	depthDecs.Usage = D3D11_USAGE_DEFAULT;
	depthDecs.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateTexture2D(&depthDecs, NULL, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDecs = {};
	depthStencilViewDecs.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDecs.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDecs.Texture2D.MipSlice = 0;

	THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateDepthStencilView(pDepthStencil.Get(), &depthStencilViewDecs, &pDepthStencilView));
}

void DepthStencilView::Bind(GFX& gfx) const noexcept
{
	GetDeviceContext(gfx)->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get());
}

void DepthStencilView::Clear(GFX& gfx)
{
	GetDeviceContext(gfx)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}