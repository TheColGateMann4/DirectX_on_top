#pragma once
#include "DepthStencilView.h"
#include "CubeTextureDrawingOrder.h"
#include "Bindable.h"
#include "RenderTarget.h"

class GFX;

class DepthTextureCube : public Bindable
{
public:
	DepthTextureCube(GFX& gfx, size_t slot);

public:
	void BindDepthTextureCubeSide(GFX& gfx, CubeTextureDrawingOrder index, DepthStencilView* depthStencilView);

	virtual void Bind(GFX& gfx) noexcept override;

private:
	std::shared_ptr<RenderTargetTextureCube> m_renderTargetViews[6];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	UINT32 m_slot;
};