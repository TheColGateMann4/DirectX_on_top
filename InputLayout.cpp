#include "InputLayout.h"

InputLayout::InputLayout(GFX& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode)
{
	HRESULT hr;

	THROW_GFX_IF_FAILED(
		GetDevice(gfx)->CreateInputLayout(
			layout.data(),
			(UINT32)std::size(layout),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&(InputLayout::pInputLayout))
	);
}

VOID InputLayout::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->IASetInputLayout(pInputLayout.Get());
}