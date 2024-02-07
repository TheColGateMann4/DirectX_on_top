#include "RenderTarget.h"
#include "DepthStencilView.h"
#include "Graphics.h"
#include "ErrorMacros.h"

RenderTarget::RenderTarget(GFX& gfx, int scale)
	:
	m_scale(scale)
{
	UpdateRenderTarget(gfx);
}

void RenderTarget::UpdateRenderTarget(GFX& gfx)
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = gfx.GetWidth() / m_scale;
	textureDesc.Height = gfx.GetHeight() / m_scale;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	THROW_GFX_IF_FAILED(
		GetDevice(gfx)->CreateTexture2D(
			&textureDesc,
			nullptr,
			&pTexture
		)
	);



	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc = {};
	textureViewDesc.Format = textureDesc.Format;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MostDetailedMip = 0;
	textureViewDesc.Texture2D.MipLevels = 1;

	THROW_GFX_IF_FAILED(
		GetDevice(gfx)->CreateShaderResourceView(
			pTexture.Get(),
			&textureViewDesc,
			&pTextureView
		)
	);



	D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
	targetViewDesc.Format = textureDesc.Format;
	targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	targetViewDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

	THROW_GFX_IF_FAILED(
		GetDevice(gfx)->CreateRenderTargetView(
			pTexture.Get(),
			&targetViewDesc,
			&pRenderTargetView
		)
	);
}

void RenderTarget::BindTexture(GFX& gfx, UINT32 slot) const noexcept
{
	GetDeviceContext(gfx)->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
}

void RenderTarget::MakeAndSetLocalViewport(GFX& gfx) noexcept
{
	D3D11_VIEWPORT viewport = {};
	viewport.Width = gfx.GetWidth() / m_scale;
	viewport.Height = gfx.GetHeight() / m_scale;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	GetDeviceContext(gfx)->RSSetViewports(1, &viewport);

	UpdateRenderTarget(gfx);
}

void RenderTarget::BindRenderTarget(GFX& gfx) noexcept
{
	MakeAndSetLocalViewport(gfx);

	GetDeviceContext(gfx)->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
}

void RenderTarget::BindRenderTarget(GFX& gfx, DepthStencilView& depthStencilView) noexcept
{
	MakeAndSetLocalViewport(gfx);

	GetDeviceContext(gfx)->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), depthStencilView.pDepthStencilView.Get());
}

void RenderTarget::ClearBuffer(GFX& gfx, DirectX::XMFLOAT4 color) const noexcept
{
	GetDeviceContext(gfx)->ClearRenderTargetView(pRenderTargetView.Get(), &color.x);
}

void RenderTarget::ChangeDownscalingRatio(int scale) noexcept
{
	m_scale = scale;
}