#pragma once
#include "GraphicResource.h"
#include <wrl.h>

class DepthStencilView : public GraphicResource
{
	friend class RenderTarget;
	friend class GFX;
public:
	DepthStencilView(GFX& gfx);

	void Bind(GFX& gfx) const noexcept;

	void Clear(GFX& gfx);

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};

