#pragma once
#include "Includes.h"
#include "Graphics.h"
#include "Bindable.h"
#include "BindableList.h"

class DepthStencil : public Bindable
{
public:
	enum StencilMode
	{
		Off, 
		Write,
		Mask
	};

public:
	DepthStencil(GFX& gfx, StencilMode mode)
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());

		if (mode == StencilMode::Write)
		{
			depthStencilDesc.StencilEnable = TRUE;
			depthStencilDesc.StencilWriteMask = 0xFF;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		}
		else if (mode == StencilMode::Mask)
		{
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.StencilEnable = TRUE;
			depthStencilDesc.StencilReadMask = 0xFF;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		}

		GetDevice(gfx)->CreateDepthStencilState(&depthStencilDesc, pDepthStencil.GetAddressOf());
	}

public:
	void Bind(GFX& gfx) noexcept override
	{
		GetDeviceContext(gfx)->OMSetDepthStencilState(pDepthStencil.Get(), 0xFF);
	}

public:
	static std::shared_ptr<DepthStencil> GetBindable(GFX& gfx, StencilMode mode)
	{
		return BindableList::GetBindable<DepthStencil>(gfx, mode);
	}

	std::string GetUID() const noexcept override
	{
		return GenerateUID(m_mode);
	};

	static std::string GetUID(StencilMode mode) noexcept
	{
		return GenerateUID(mode);
	};

private:
	static std::string GenerateUID(StencilMode mode)
	{
		return std::to_string((size_t)mode);
	}

protected:
	StencilMode m_mode;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencil;
};

