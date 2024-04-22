#pragma once
#include <d3d11.h>
#include "GraphicBuffer.h"
#include "Bindable.h"
#include "wrl_no_warnings.h"

class DepthStencilView : public GraphicBuffer, public Bindable
{
	friend class RenderTarget;
	friend class GFX;

public:
	enum Mode
	{
		StencilAndDepth,
		DepthOnly
	};

public:
	DepthStencilView();

	DepthStencilView(GFX& gfx, Mode depthStencilViewMode = StencilAndDepth, bool isForcedRatio = false);

public:
	virtual void BindRenderTarget(GFX& gfx, GraphicBuffer* graphicBuffer = nullptr) override;

	void Bind(GFX& gfx) noexcept;

	virtual void GetBuffer(struct ID3D11Resource** resource) override;

	virtual void Clear(GFX& gfx) const override;

	static DXGI_FORMAT GetTypelessFormat(Mode depthStencilViewMode);

	static DXGI_FORMAT GetTypedFormat(Mode depthStencilViewMode);

	static DXGI_FORMAT GetColorTypeFormat(Mode depthStencilViewMode);

protected:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};

class DepthStencilViewWithTexture : public DepthStencilView
{
public:
	DepthStencilViewWithTexture(GFX& gfx, size_t slot, Mode depthStencilViewMode = StencilAndDepth);

public:
	virtual void Bind(GFX& gfx) noexcept override;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
	size_t m_slot;
};

class DepthStencilViewTextureCube : public DepthStencilView
{
public:
	DepthStencilViewTextureCube();

	DepthStencilViewTextureCube(GFX& gfx, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, size_t index);

public:
	virtual void Bind(GFX& gfx) noexcept override;
};


