#pragma once
#include <d3d11.h>
#include "GraphicBuffer.h"
#include "wrl_no_warnings.h"

class DepthStencilView : public GraphicBuffer
{
	friend class RenderTarget;
	friend class GFX;
public:
	DepthStencilView(GFX& gfx);

	virtual void BindRenderTarget(GFX& gfx, GraphicBuffer* graphicBuffer = nullptr) override;

	virtual void GetBuffer(struct ID3D11Resource** resource) override;

	virtual void Clear(GFX& gfx) const override;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};

