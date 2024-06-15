#include "RenderTarget.h"
#include "DepthStencilView.h"
#include "Graphics.h"
#include "ErrorMacros.h"

RenderTarget::RenderTarget()
	:
	m_width(0), m_height(0), m_isBackBuffer(false), m_isTextureRenderTarget(false), m_initialized(false)
{
}

RenderTarget::RenderTarget(GFX& gfx, const int width, const int height, bool isTextureRenderTarget)
	:
	m_width(width), m_height(height), m_isBackBuffer(false), m_isTextureRenderTarget(isTextureRenderTarget), m_initialized(true)
{
	Update(gfx);
}

void RenderTarget::Update(GFX& gfx)
{
	if (!m_initialized)
		THROW_INTERNAL_ERROR("Tried to Update uninitialized RenderTarget");

	HRESULT hr;

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

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pRTTexture;

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateTexture2D(
			&textureDesc,
			nullptr,
			&pRTTexture
		)
	);

	D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
	targetViewDesc.Format = textureDesc.Format;
	targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	targetViewDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateRenderTargetView(
			pRTTexture.Get(),
			&targetViewDesc,
			&m_pRenderTargetView
		)
	);
}

RenderTarget::RenderTarget(GFX& gfx, Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture)
	:
	m_isBackBuffer(true), // setting it this way since we only use it for backBuffer for now
	m_isTextureRenderTarget(false), // m_isTextureRenderTarget means that it binds texture, not that it uses one to create RenderTargetView
	m_initialized(true)
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
	this->m_initialized = renderTarget.m_initialized;
}

void RenderTarget::MakeAndSetLocalViewport(GFX& gfx)
{
	if (!m_initialized)
		THROW_INTERNAL_ERROR("Tried to update viewport to uninitialized RenderTarget");

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
	if (!m_initialized)
		THROW_INTERNAL_ERROR("Tried to get buffer from uninitialized RenderTarget");

	m_pRenderTargetView->GetResource(resource);
}

void RenderTarget::Bind(GFX& gfx) noexcept
{

}

void RenderTarget::BindRenderTarget(GFX& gfx, GraphicBuffer* graphicBuffer)
{
	if (!m_initialized)
		THROW_INTERNAL_ERROR("Tried to bind uninitialized RenderTarget");

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
	if (!m_initialized)
		THROW_INTERNAL_ERROR("Tried to clear uninitialized RenderTarget");

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
	this->m_pRTTexture = renderTarget.m_pRTTexture;
	this->m_pSRTexture = renderTarget.m_pSRTexture;
}

RenderTargetWithTexture::RenderTargetWithTexture(GFX& gfx, const int width, const int height, int slot)
	: 
	RenderTarget(gfx, width, height, true),
	m_slot(slot)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC renderTargetTextureDesc = {};

	{
		Microsoft::WRL::ComPtr<ID3D11Resource> pRTResource;

		m_pRenderTargetView->GetResource(&pRTResource);

		pRTResource->QueryInterface(m_pRTTexture.GetAddressOf());


		m_pRTTexture->GetDesc(&renderTargetTextureDesc);
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = m_width;
	textureDesc.Height = m_height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = renderTargetTextureDesc.Format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateTexture2D(
			&textureDesc,
			nullptr,
			&m_pSRTexture
		)
	);

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc = {};
	textureViewDesc.Format = renderTargetTextureDesc.Format;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MostDetailedMip = 0;
	textureViewDesc.Texture2D.MipLevels = 1;

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateShaderResourceView(
			m_pSRTexture.Get(),
			&textureViewDesc,
			&m_pTextureView
		)
	);
}

void RenderTargetWithTexture::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->CopyResource(m_pSRTexture.Get(), m_pRTTexture.Get());

	GFX::GetDeviceContext(gfx)->PSSetShaderResources(m_slot, 1, m_pTextureView.GetAddressOf());
}



RenderTargetTextureCube::RenderTargetTextureCube()
{

}

RenderTargetTextureCube::RenderTargetTextureCube(GFX& gfx, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, size_t index)
{
	m_isTextureRenderTarget = true;
	m_initialized = true;

	D3D11_TEXTURE2D_DESC textureDesc;
	pTexture->GetDesc(&textureDesc);

	m_width = textureDesc.Width;
	m_height = textureDesc.Height;

	HRESULT hr;

	D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
	targetViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	targetViewDesc.Texture2DArray.ArraySize = 1;
	targetViewDesc.Texture2DArray.FirstArraySlice = index;
	targetViewDesc.Texture2DArray.MipSlice = 0;

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateRenderTargetView(
			pTexture.Get(),
			&targetViewDesc,
			&m_pRenderTargetView
		)
	);
}

void RenderTargetTextureCube::Bind(GFX& gfx) noexcept
{
	// we are not binding this one since it will be bound by CubeTexture object
}