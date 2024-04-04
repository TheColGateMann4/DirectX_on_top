#pragma once
#include "GraphicResource.h"
#include "GraphicBuffer.h"
#include "wrl_no_warnings.h"

class DepthStencilView :public GraphicResource, public GraphicBuffer
{
	friend class RenderTarget;
	friend class GFX;
public:
	DepthStencilView(GFX& gfx);

	virtual void BindRenderTarget(GFX& gfx, GraphicBuffer* graphicBuffer = nullptr) override;

	virtual void Clear(GFX& gfx) const override;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};

