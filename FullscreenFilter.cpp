#include "FullscreenFilter.h"
#include "DynamicConstantBuffer.h"
#include "Bindable.h"
#include "BindableClassesMacro.h"
#include "Graphics.h"

FullscreenFilter::FullscreenFilter(GFX& gfx)
{
	DynamicVertex::VertexLayout vertexLayout;
	vertexLayout.Append(DynamicVertex::VertexLayout::Position2D);

	DynamicVertex::VertexBuffer vertexBuffer(std::move(vertexLayout));
	vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ -1.0f, 1.0f });
	vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ 1.0f, 1.0f });
	vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ -1.0f, -1.0f });
	vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ 1.0f, -1.0f });

	std::vector<UINT32> indices = { 0,1,2,1,3,2 };


	m_bindables.push_back(VertexBuffer::GetBindable(gfx, "FullScreen", vertexBuffer));
	m_bindables.push_back(IndexBuffer::GetBindable(gfx, "FullScreen", indices));

	pIndexBuffer = dynamic_cast<IndexBuffer*>(m_bindables.back().get());

	m_bindables.push_back(VertexShader::GetBindable(gfx, "VS_FullScreen.cso"));

	VertexShader* pVertexShader = dynamic_cast<VertexShader*>(m_bindables.back().get());

	m_bindables.push_back(PixelShader::GetBindable(gfx, "PS_Fullscreen_NegativeColors.cso"));
	m_bindables.push_back(InputLayout::GetBindable(gfx, vertexBuffer.GetLayout().GetDirectXLayout(), pVertexShader->GetByteCode()));
}

void FullscreenFilter::Render(GFX& gfx) const noexcept
{
	for (const auto& bindable : m_bindables)
		bindable->Bind(gfx);

	gfx.DrawIndexed(pIndexBuffer->GetCount());
}