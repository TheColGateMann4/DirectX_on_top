#pragma once
#include "DepthStencilView.h"
#include "CubeTextureDrawingOrder.h"
#include "Bindable/Bindable.h"
#include "RenderTarget.h"

class GFX;

class DepthTextureCube : public Bindable
{
public:
	DepthTextureCube(GFX& gfx, size_t slot);

public:
	std::shared_ptr<DepthStencilView> GetDepthTextureCubeSide(CubeTextureDrawingOrder index);

	virtual void Bind(GFX& gfx) noexcept override;

private:
	std::shared_ptr<DepthStencilViewTextureCube> m_depthStencilViews[6];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	UINT32 m_slot;
};