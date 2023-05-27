#pragma once
#include "Includes.h"
#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(GFX& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode);
	VOID Bind(GFX& gfx) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};
