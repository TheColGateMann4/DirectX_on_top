#include "InputLayout.h"

InputLayout::InputLayout(GFX& gfx, const DynamicVertex::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode)
	: m_layout(layout)
{
	HRESULT hr;
	const std::vector<D3D11_INPUT_ELEMENT_DESC> directXLayout = m_layout.GetDirectXLayout();

	THROW_GFX_IF_FAILED(
		GetDevice(gfx)->CreateInputLayout(
			directXLayout.data(),
			(UINT32)directXLayout.size(),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&pInputLayout)
	);
}

InputLayout::InputLayout(GFX& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode)
{
	HRESULT hr;

	THROW_GFX_IF_FAILED(
		GetDevice(gfx)->CreateInputLayout(
			layout.data(),
			(UINT32)layout.size(),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&pInputLayout)
	);
}

VOID InputLayout::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->IASetInputLayout(pInputLayout.Get());
}