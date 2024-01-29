#pragma once
#include "GraphicResource.h"
#include <wrl.h>

class RenderTarget : public GraphicResource
{
public:
	RenderTarget(GFX& gfx);

public:
	void BindTexture(GFX& gfx, UINT32 slot) const noexcept;

	void BindRenderTarget(GFX& gfx) const noexcept;

	void BindRenderTarget(GFX& gfx, class DepthStencilView& depthStencilView) const noexcept;
	
public:
	void ClearBuffer(GFX& gfx, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4{0.0f, 0.0f, 0.0f, 0.0f}) const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;
};

