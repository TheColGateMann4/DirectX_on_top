#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "BindableList.h"
#include "Vertex.h"
#include "VertexShader.h"

class InputLayout : public Bindable
{
public:
	InputLayout(GFX& gfx, const DynamicVertex::VertexLayout& layout, VertexShader* pVertexShader);

	InputLayout(GFX& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, VertexShader* pVertexShader);;

public:
	VOID Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<InputLayout> GetBindable(GFX& gfx, const DynamicVertex::VertexLayout& layout, VertexShader* pVertexShader)
	{
		return BindableList::GetBindable<InputLayout>(gfx, layout, pVertexShader);
	}
	static std::shared_ptr<InputLayout> GetBindable(GFX& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, VertexShader* pVertexShader)
	{
		return BindableList::GetBindable<InputLayout>(gfx, layout, pVertexShader);
	}

	std::string GetLocalUID() const noexcept override 
	{ 
		return GenerateUID(m_layout, m_vertexShaderName);
	};

	static std::string GetStaticUID(const DynamicVertex::VertexLayout& layout, VertexShader* pVertexShader) noexcept
	{
		return GenerateUID(layout, pVertexShader->GetLocalUID());
	};
	static std::string GetStaticUID(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, VertexShader* pVertexShader) noexcept
	{
		return GenerateUID(layout, pVertexShader->GetLocalUID());
	};

private:
	static std::string GenerateUID(const DynamicVertex::VertexLayout& layout, std::string vertexShaderUID)
	{ 
		return layout.GetUID() + '@' + vertexShaderUID;
	}
	static std::string GenerateUID(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, std::string vertexShaderUID)
	{
		std::string result = {};
		for (const D3D11_INPUT_ELEMENT_DESC& element : layout)
			result += element.SemanticName;
		return result + '@' + vertexShaderUID;
	}

protected:
	DynamicVertex::VertexLayout m_layout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	std::string m_vertexShaderName;
};
