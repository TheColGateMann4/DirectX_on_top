#include "DepthStencilView.h"
#include "RenderTarget.h"
#include "Graphics.h"
#include "Includes.h"
#include "ErrorMacros.h"

DepthStencilView::DepthStencilView()
{

}

DepthStencilView::DepthStencilView(GFX& gfx, Mode depthStencilViewMode, bool isForcedRatio)
{
	HRESULT hr;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;

	{
		D3D11_TEXTURE2D_DESC depthDecs = {};
		depthDecs.Width = gfx.GetWidth();
		depthDecs.Height = (isForcedRatio) ? gfx.GetWidth() : gfx.GetHeight();
		depthDecs.MipLevels = 1;
		depthDecs.ArraySize = 1;
		depthDecs.Format = GetTypelessFormat(depthStencilViewMode);
		depthDecs.SampleDesc.Count = 1;
		depthDecs.SampleDesc.Quality = 0;
		depthDecs.Usage = D3D11_USAGE_DEFAULT;
		depthDecs.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		if (depthStencilViewMode == DepthOnly)
			depthDecs.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

		THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateTexture2D(&depthDecs, NULL, &pDepthStencil));
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDecs = {};
		depthStencilViewDecs.Format = GetTypedFormat(depthStencilViewMode);
		depthStencilViewDecs.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDecs.Texture2D.MipSlice = 0;

		THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateDepthStencilView(pDepthStencil.Get(), &depthStencilViewDecs, &pDepthStencilView));
	}
}

void DepthStencilView::BindRenderTarget(GFX& gfx, GraphicBuffer* graphicBuffer)
{
	THROW_INTERNAL_ERROR_IF("Tried to bind null depthStencilView", pDepthStencilView == nullptr);

	if (graphicBuffer == nullptr)
	{
		THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get()));
	}
	else if(RenderTarget* renderTarget = dynamic_cast<RenderTarget*>(graphicBuffer))
	{
		renderTarget->BindRenderTarget(gfx, this);
	}
	else
	{
		std::string errorString = "DepthStencilView object got wrong object passed to function BindRenderTarget(),\n local object class name: \"";
		errorString += typeid(*this).name();
		errorString += "\". Passed object class name: \"";
		errorString += typeid(*graphicBuffer).name();
		errorString += "\".";

		THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
	}
}

void DepthStencilView::Bind(GFX& gfx) noexcept
{

}

void DepthStencilView::GetBuffer(ID3D11Resource** resource)
{
	pDepthStencilView->GetResource(resource);
}

void DepthStencilView::Clear(GFX& gfx) const
{
	GFX::GetDeviceContext(gfx)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

constexpr DXGI_FORMAT DepthStencilView::GetTypelessFormat(DepthStencilView::Mode depthStencilViewMode)
{
	if (depthStencilViewMode == StencilAndDepth)
		return DXGI_FORMAT_R24G8_TYPELESS;
	if (depthStencilViewMode == DepthOnly)
		return DXGI_FORMAT_R32_TYPELESS;

	std::string errorString = "Used unhandled depthStencilViewMode. Mode was: \"";
	errorString += std::to_string(depthStencilViewMode);
	errorString += "\".";

	THROW_INTERNAL_ERROR(errorString.c_str());
}

constexpr DXGI_FORMAT DepthStencilView::GetTypedFormat(DepthStencilView::Mode depthStencilViewMode)
{
	if (depthStencilViewMode == StencilAndDepth)
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (depthStencilViewMode == DepthOnly)
		return DXGI_FORMAT_D32_FLOAT;

	std::string errorString = "Used unhandled depthStencilViewMode. Mode was: \"";
	errorString += std::to_string(depthStencilViewMode);
	errorString += "\".";

	THROW_INTERNAL_ERROR(errorString.c_str());
}

constexpr DXGI_FORMAT DepthStencilView::GetColorTypeFormat(DepthStencilView::Mode depthStencilViewMode)
{
	if (depthStencilViewMode == StencilAndDepth)
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	if (depthStencilViewMode == DepthOnly)
		return DXGI_FORMAT_R32_FLOAT;

	std::string errorString = "Used unhandled depthStencilViewMode. Mode was: \"";
	errorString += std::to_string(depthStencilViewMode);
	errorString += "\".";

	THROW_INTERNAL_ERROR(errorString.c_str());
}



DepthStencilViewWithTexture::DepthStencilViewWithTexture(GFX& gfx, size_t slot, Mode depthStencilViewMode)
	: 
	DepthStencilView(gfx, depthStencilViewMode),
	m_slot(slot)
{
	HRESULT hr;

	Microsoft::WRL::ComPtr<ID3D11Resource> pDepthStencilTexture;

	pDepthStencilView->GetResource(&pDepthStencilTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc = {};
	textureViewDesc.Format = GetColorTypeFormat(depthStencilViewMode);
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MostDetailedMip = 0;
	textureViewDesc.Texture2D.MipLevels = 1;

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateShaderResourceView(
			pDepthStencilTexture.Get(),
			&textureViewDesc,
			&m_pTextureView
		)
	);
}

void DepthStencilViewWithTexture::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->PSSetShaderResources(m_slot, 1, m_pTextureView.GetAddressOf());
}

DepthStencilViewTextureCube::DepthStencilViewTextureCube()
{

}

DepthStencilViewTextureCube::DepthStencilViewTextureCube(GFX& gfx, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, size_t index)
{
	HRESULT hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDecs = {};
	depthStencilViewDecs.Format = GetTypedFormat(DepthOnly);
	depthStencilViewDecs.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthStencilViewDecs.Texture2DArray.ArraySize = 1;
	depthStencilViewDecs.Texture2DArray.FirstArraySlice = index;
	depthStencilViewDecs.Texture2DArray.MipSlice = 0;

	THROW_GFX_IF_FAILED(
		GFX::GetDevice(gfx)->CreateDepthStencilView(
			pTexture.Get(),
			&depthStencilViewDecs,
			&pDepthStencilView
		)
	);
}

void DepthStencilViewTextureCube::Bind(GFX& gfx) noexcept
{
	// we are not binding this one since it will be bound by CubeTexture object
}