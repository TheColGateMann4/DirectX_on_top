#include "FullscreenFilter.h"
#include "DynamicConstantBuffer.h"
#include "Bindable.h"
#include "BindableClassesMacro.h"
#include "Graphics.h"

FullscreenFilter::FullscreenFilter(GFX& gfx)
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

	pIndexBuffer = dynamic_cast<IndexBuffer*>(m_bindables.back().get());

	{
		DynamicConstantBuffer::BufferLayout constBufferLayout;
		constBufferLayout.Add<DynamicConstantBuffer::DataType::Int>("strength");

		DynamicConstantBuffer::BufferData constBufferData(constBufferLayout);
		*constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("strength") = 3;

		m_bindables.push_back(std::make_shared<CachedBuffer>(gfx, constBufferData, 0, true));
	}


	m_bindables.push_back(VertexShader::GetBindable(gfx, "VS_FullScreen.cso"));

	VertexShader* pVertexShader = dynamic_cast<VertexShader*>(m_bindables.back().get());

	m_bindables.push_back(PixelShader::GetBindable(gfx, "PS_Fullscreen_Normal.cso"));
	m_bindables.push_back(InputLayout::GetBindable(gfx, vertexLayout.GetDirectXLayout(), pVertexShader->GetByteCode()));
	m_bindables.push_back(SamplerState::GetBindable(gfx, false, true));
	m_bindables.push_back(BlendState::GetBindable(gfx, true));
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

	for (size_t i = 0; i < m_bindables.size(); i++)
		if (CachedBuffer* pCachedBuffer = dynamic_cast<CachedBuffer*>(m_bindables.at(i).get()))
		{
			DynamicConstantBuffer::BufferData bufferData = pCachedBuffer->constBufferData;
			*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("strength") = strength;

			pCachedBuffer->Update(gfx, bufferData);
		}
}

void FullscreenFilter::Render(GFX& gfx) const noexcept
{
	for (const auto& bindable : m_bindables)
		bindable->Bind(gfx);

	gfx.DrawIndexed(pIndexBuffer->GetCount());
}