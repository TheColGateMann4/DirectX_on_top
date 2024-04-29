#pragma once
#include "GraphicBuffer.h"
#include "Bindable.h"
#include "wrl_no_warnings.h"

class RenderTarget : public Bindable, public GraphicBuffer
{
public:
	RenderTarget();
	RenderTarget(GFX& gfx, const int width, const int height, bool isTextureRenderTarget = false);
	RenderTarget(GFX& gfx, Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture);
	RenderTarget(const RenderTarget& renderTarget);

public:
	void Update(GFX& gfx);

	virtual void Bind(GFX& gfx) noexcept override;
	virtual void BindRenderTarget(GFX& gfx, GraphicBuffer* graphicBuffer = nullptr) override;

public:
	virtual void Clear(GFX& gfx) const override;

	void ChangeResolution(GFX& gfx, const int width, const int height) noexcept;

private:
	void MakeAndSetLocalViewport(GFX& gfx);

	virtual void GetBuffer(struct ID3D11Resource** resource) override;

protected:
	int m_width;
	int m_height;
	bool m_isBackBuffer;
	bool m_isTextureRenderTarget;
	bool m_initialized;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
};

class RenderTargetWithTexture : public RenderTarget
{
public:
	RenderTargetWithTexture(const RenderTargetWithTexture& renderTarget);
	RenderTargetWithTexture(GFX& gfx, const int width, const int height, int slot);

public:
	virtual void Bind(GFX& gfx) noexcept override;

private:
	int m_slot;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
};

class RenderTargetTextureCube : public RenderTarget
{
public:
	RenderTargetTextureCube();

	RenderTargetTextureCube(GFX& gfx, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, size_t index);

public:
	virtual void Bind(GFX& gfx) noexcept override;
};