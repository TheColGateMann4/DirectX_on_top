#include "FullscreenFilterRenderPass.h"
#include "BindableClassesMacro.h"
#include "Graphics.h"

FullscreenFilterRenderPass::FullscreenFilterRenderPass(class GFX& gfx, const char* name)
	: RenderBindablePass(name)
{
	DynamicVertex::VertexLayout vertexLayout;
	{
		vertexLayout.Append(DynamicVertex::VertexLayout::Position2D);

		DynamicVertex::VertexBuffer vertexBuffer(vertexLayout);
		vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ -1.0f, 1.0f });
		vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ 1.0f, 1.0f });
		vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ -1.0f, -1.0f });
		vertexBuffer.Emplace_Back(DirectX::XMFLOAT2{ 1.0f, -1.0f });

		AddBindable(VertexBuffer::GetBindable(gfx, "FullScreen", vertexBuffer));
	}

	{
		std::vector<UINT32> indices = { 0,1,2,1,3,2 };

		AddBindable(IndexBuffer::GetBindable(gfx, "FullScreen", indices));
	}

	std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_FullScreen.cso");

	AddBindable(PixelShader::GetBindable(gfx, "PS_Fullscreen_Normal.cso"));
	AddBindable(InputLayout::GetBindable(gfx, vertexLayout.GetDirectXLayout(), pVertexShader.get()));
	AddBindable(SamplerState::GetBindable(gfx, SamplerState::Mode::MIRROR, 0, SamplerState::NEVER, SamplerState::POINT));
	AddBindable(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBindable(pVertexShader);
}

void FullscreenFilterRenderPass::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	RenderBindablePass::Render(gfx);
	gfx.DrawIndexed(6);
}