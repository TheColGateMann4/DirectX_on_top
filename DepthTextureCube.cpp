#include "DepthTextureCube.h"
#include "Graphics.h"
#include <DirectXTex.h>
#include "ErrorMacros.h"

DepthTextureCube::DepthTextureCube(GFX& gfx, size_t slot)
	: 
	m_slot(slot)
{
	HRESULT hr;
	using namespace DirectX;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pCubeTexture;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Height = textureDesc.Width = gfx.GetWidth(); // since we use textureCube texture every image has to be in 1:1 ratio
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pCubeTexture));

	for (size_t i = 0; i < 6; i++)
	{
		m_depthStencilViews[i] = std::make_shared<DepthStencilView>(gfx, DepthStencilView::Mode::DepthOnly, true);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceViewDesc.TextureCube.MipLevels = 1;
	shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(pCubeTexture.Get(), &shaderResourceViewDesc, &pShaderResourceView));
}

void DepthTextureCube::BindDepthTextureCubeSide(GFX& gfx, CubeTextureDrawingOrder index, RenderTarget* renderTarget)
{
	int iIndex = static_cast<int>(index);
	assert(iIndex < 6 && iIndex >= 0);

	m_depthStencilViews[iIndex]->Clear(gfx);

	m_depthStencilViews[iIndex]->BindRenderTarget(gfx, renderTarget);
}

void DepthTextureCube::Bind(GFX& gfx) noexcept
{
	ID3D11Resource* destBuffer = nullptr;
	pShaderResourceView->GetResource(&destBuffer);

	for (size_t i = 0; i < 6; i++)
	{
		ID3D11Resource* sourceBuffer = nullptr;
		m_depthStencilViews[i]->GetBuffer(&sourceBuffer);

		GFX::GetDeviceContext(gfx)->CopySubresourceRegion(destBuffer, i, 0,0,0, sourceBuffer, 0, NULL);
	}

	GFX::GetDeviceContext(gfx)->PSSetShaderResources(m_slot, 1, pShaderResourceView.GetAddressOf());
}