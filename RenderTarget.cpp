#include "RenderTarget.h"
#include "DepthStencilView.h"
#include "Graphics.h"
#include "ErrorMacros.h"

RenderTarget::RenderTarget(GFX& gfx, const int width, const int height, bool isTextureRenderTarget)
	:
	m_width(width), m_height(height), m_isBackBuffer(false), m_isTextureRenderTarget(isTextureRenderTarget)
{
	Update(gfx);
}

void RenderTarget::Update(GFX& gfx)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture = nullptr;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = m_width;
	textureDesc.Height = m_height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (m_isTextureRenderTarget) textureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateTexture2D(
			&textureDesc,
			nullptr,
			&pTexture
		)
	);

	D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
	targetViewDesc.Format = textureDesc.Format;
	targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	targetViewDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateRenderTargetView(
			pTexture.Get(),
			&targetViewDesc,
			&m_pRenderTargetView
		)
	);
}

RenderTarget::RenderTarget(GFX& gfx, Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture)
	:
	m_isBackBuffer(true), // setting it this way since we only use it for backBuffer for now
	m_isTextureRenderTarget(false) // m_isTextureRenderTarget means that it binds texture, not that it uses one to create RenderTargetView
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	static_cast<ID3D11Texture2D*>(pTexture.Get())->GetDesc(&textureDesc);
	m_height = textureDesc.Height;
	m_width = textureDesc.Width;

	D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
	targetViewDesc.Format = textureDesc.Format;
	targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	targetViewDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateRenderTargetView(
			pTexture.Get(),
			&targetViewDesc,
			&m_pRenderTargetView
		)
	);
}

RenderTarget::RenderTarget(const RenderTarget& renderTarget)
{
	this->m_width = renderTarget.m_width;
	this->m_height = renderTarget.m_height;
	this->m_isBackBuffer = renderTarget.m_isBackBuffer;
	this->m_isTextureRenderTarget = renderTarget.m_isTextureRenderTarget;
	this->m_pRenderTargetView = renderTarget.m_pRenderTargetView;
}

void RenderTarget::MakeAndSetLocalViewport(GFX& gfx)
{
	D3D11_VIEWPORT viewport = {};
	viewport.Width = m_width;
	viewport.Height = m_height;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->RSSetViewports(1, &viewport));

	if(!m_isTextureRenderTarget && !m_isBackBuffer)
		Update(gfx);
}

void RenderTarget::GetBuffer(ID3D11Resource** resource)
{
	m_pRenderTargetView->GetResource(resource);
}

void RenderTarget::Bind(GFX& gfx) noexcept
{

}

void RenderTarget::BindRenderTarget(GFX& gfx, GraphicBuffer* graphicBuffer)
{
	MakeAndSetLocalViewport(gfx);
	
	ID3D11DepthStencilView* usedDepthStencilView = nullptr;

	if(graphicBuffer != nullptr)
	{
		if (DepthStencilView* depthStencilView = dynamic_cast<DepthStencilView*>(graphicBuffer))
		{
			usedDepthStencilView = depthStencilView->pDepthStencilView.Get();
		}
		else
		{
			std::string errorString = "RenderTarget object got wrong object passed to function BindRenderTarget(),\n local object class name: \"";
			errorString += typeid(*this).name();
			errorString += "\". Passed object class name: \"";
			errorString += typeid(*graphicBuffer).name();
			errorString += "\".";

			THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
		}
	}

	THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), usedDepthStencilView));
}

void RenderTarget::Clear(GFX& gfx) const
{
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4{ 0.0f, 0.0f, 0.0f, 0.0f };
	THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->ClearRenderTargetView(m_pRenderTargetView.Get(), &color.x));
}

void RenderTarget::ChangeResolution(GFX& gfx, const int width, const int height) noexcept
{
	m_width = width;
	m_height = height;

	Update(gfx);
}


RenderTargetWithTexture::RenderTargetWithTexture(const RenderTargetWithTexture& renderTarget)
	: 
	RenderTarget(renderTarget)
{
	this->m_slot = renderTarget.m_slot;
	this->m_pTextureView = renderTarget.m_pTextureView;
}

RenderTargetWithTexture::RenderTargetWithTexture(GFX& gfx, const int width, const int height, int slot)
	: 
	RenderTarget(gfx, width, height, true),
	m_slot(slot)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Resource> pTexture = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};

	m_pRenderTargetView->GetResource(pTexture.GetAddressOf());

	m_pRenderTargetView->GetDesc(&renderTargetViewDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc = {};
	textureViewDesc.Format = renderTargetViewDesc.Format;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MostDetailedMip = 0;
	textureViewDesc.Texture2D.MipLevels = 1;

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateShaderResourceView(
			pTexture.Get(),
			&textureViewDesc,
			&m_pTextureView
		)
	);
}

void RenderTargetWithTexture::Bind(GFX& gfx) noexcept
{
	THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->PSSetShaderResources(m_slot, 1, m_pTextureView.GetAddressOf()));
}