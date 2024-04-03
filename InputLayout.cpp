#include "InputLayout.h"
#include "ErrorMacros.h"
#include "VertexShader.h"

InputLayout::InputLayout(GFX& gfx, const DynamicVertex::VertexLayout& layout, VertexShader* pVertexShader)
	: 
	m_layout(layout),
	m_vertexShaderName(pVertexShader->GetLocalUID())
{
	HRESULT hr;
	const std::vector<D3D11_INPUT_ELEMENT_DESC> directXLayout = m_layout.GetDirectXLayout();
	ID3DBlob* pVertexShaderByteCode = pVertexShader->GetByteCode();

	THROW_GFX_IF_FAILED(
		GetDevice(gfx)->CreateInputLayout(
			directXLayout.data(),
			(UINT32)directXLayout.size(),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&pInputLayout)
	);
}

InputLayout::InputLayout(GFX& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, VertexShader* pVertexShader)
{
	HRESULT hr;
	ID3DBlob* pVertexShaderByteCode = pVertexShader->GetByteCode();

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