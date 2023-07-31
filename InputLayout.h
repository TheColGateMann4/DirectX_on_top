#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "BindableList.h"
#include "Vertex.h"

class InputLayout : public Bindable
{
public:
	InputLayout(GFX& gfx, const DynamicVertex::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode);

	InputLayout(GFX& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode);;

public:
	VOID Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<InputLayout> GetBindable(GFX& gfx, const DynamicVertex::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode)
	{
		return BindableList::GetBindable<InputLayout>(gfx, layout, pVertexShaderByteCode);
	}
	static std::shared_ptr<InputLayout> GetBindable(GFX& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode)
	{
		return BindableList::GetBindable<InputLayout>(gfx, layout, pVertexShaderByteCode);
	}

	std::string GetUID() const noexcept override 
	{ 
		return GenerateUID(m_layout); 
	};

	static std::string GetUID(const DynamicVertex::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode = nullptr) noexcept
	{
		return GenerateUID(layout);
	};
	static std::string GetUID(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode = nullptr) noexcept
	{
		return GenerateUID(layout);
	};

private:
	static std::string GenerateUID(const DynamicVertex::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode = nullptr)
	{ 
		return layout.GetUID(); 
	}
	static std::string GenerateUID(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode = nullptr)
	{
		std::string result = {};
		for (const D3D11_INPUT_ELEMENT_DESC& element : layout)
			result += element.SemanticName;
		return result;
	}

protected:
	DynamicVertex::VertexLayout m_layout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	
};
