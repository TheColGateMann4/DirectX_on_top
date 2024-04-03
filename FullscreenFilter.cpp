#include "FullscreenFilter.h"
#include "DynamicConstantBuffer.h"
#include "Bindable.h"
#include "BindableClassesMacro.h"
#include "Graphics.h"

FullscreenFilter::FullscreenFilter(GFX& gfx)
	:
	m_gaussFilter(gfx, 7, 2.6f)
{
	DynamicVertex::VertexLayout vertexLayout;
	{
		vertexLayout.Append(DynamicVertex::VertexLayout::Position2D);

		DynamicVertex::VertexBuffer vertexBuffer(vertexLayout);
		vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ -1.0f, 1.0f });
		vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ 1.0f, 1.0f });
		vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ -1.0f, -1.0f });
		vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ 1.0f, -1.0f });

		m_bindables.push_back(VertexBuffer::GetBindable(gfx, "FullScreen", vertexBuffer));
	}

	{
		std::vector<UINT32> indices = { 0,1,2,1,3,2 };

		m_bindables.push_back(IndexBuffer::GetBindable(gfx, "FullScreen", indices));
	}

	{
		DynamicConstantBuffer::BufferLayout constBufferLayout;
		constBufferLayout.Add<DynamicConstantBuffer::DataType::Int>("strength");

		DynamicConstantBuffer::BufferData constBufferData(constBufferLayout);
		*constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("strength") = 3;

		m_constBuffer = std::make_shared<CachedBuffer>(gfx, constBufferData, 0, true);
	}

	pIndexBuffer = dynamic_cast<IndexBuffer*>(m_bindables.back().get());

	m_bindables.push_back(VertexShader::GetBindable(gfx, "VS_FullScreen.cso"));

	VertexShader* pVertexShader = dynamic_cast<VertexShader*>(m_bindables.back().get());

	m_bindables.push_back(PixelShader::GetBindable(gfx, "PS_Fullscreen_Normal.cso"));
	m_bindables.push_back(InputLayout::GetBindable(gfx, vertexLayout.GetDirectXLayout(), pVertexShader));
	m_bindables.push_back(SamplerState::GetBindable(gfx, false, true));
}

void FullscreenFilter::ChangePixelShader(GFX& gfx, std::string ShaderName)
{
	currentShaderName = ShaderName;

	for (size_t i = 0; i < m_bindables.size(); i++)
		if (PixelShader* pPixelShader = dynamic_cast<PixelShader*>(m_bindables.at(i).get()))
			m_bindables.erase(m_bindables.begin() + i);

	m_bindables.push_back(PixelShader::GetBindable(gfx, ("PS_Fullscreen_" + ShaderName + ".cso").c_str()));
}

void FullscreenFilter::ChangeBlurStrength(GFX& gfx, int strength)
{
	if (currentShaderName != "Blur" && currentShaderName != "Outline")
		return;

	DynamicConstantBuffer::BufferData bufferData = m_constBuffer->constBufferData;
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("strength") = strength;

	m_constBuffer->Update(gfx, bufferData);
}

void FullscreenFilter::Bind(GFX& gfx) const noexcept
{
	for (const auto& bindable : m_bindables)
		bindable->Bind(gfx);
}

void FullscreenFilter::Draw(GFX& gfx) const
{
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void FullscreenFilter::BindGaussBlur(class GFX& gfx)
{
	m_gaussFilter.Bind(gfx);
}

GaussBlurFilter* FullscreenFilter::GetGuassBlurFilter()
{
	return &m_gaussFilter;
}