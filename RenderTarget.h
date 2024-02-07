#pragma once
#include "GraphicResource.h"
#include <wrl.h>

class RenderTarget : public GraphicResource
{
public:
	RenderTarget(GFX& gfx, int scale);

public:
	void UpdateRenderTarget(GFX& gfx);

public:
	void BindTexture(GFX& gfx, UINT32 slot) const noexcept;

	void BindRenderTarget(GFX& gfx) noexcept;

	void BindRenderTarget(GFX& gfx, class DepthStencilView& depthStencilView) noexcept;

public:
	void ClearBuffer(GFX& gfx, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4{0.0f, 0.0f, 0.0f, 0.0f}) const noexcept;

	void ChangeDownscalingRatio(int scale) noexcept;

private:
	void MakeAndSetLocalViewport(GFX& gfx) noexcept;

private:
	int m_scale;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;
};

