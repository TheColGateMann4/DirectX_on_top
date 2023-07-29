#include "PointLightModel.h"
#include "BindableClassesMacro.h"
#include "SimpleMesh.h"
#include "Sphere.h"

PointLightModel::PointLightModel(GFX& gfx, float radius)
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};
	SimpleMesh<Vertex> model = Sphere::GetMesh<Vertex>(35, 35);
	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
	AddBindable(std::make_unique<VertexBuffer>(gfx, model.m_vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_indices));

	auto pvs = std::make_unique<VertexShader>(gfx, "VertexShader.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBindable(std::move(pvs));

	AddBindable(std::make_unique<PixelShader>(gfx, "PixelSolidColorShader.cso"));

	__declspec(align(16))
		struct PSColorConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
	} colorConstBuffer;
	AddBindable(std::make_unique<PixelConstantBuffer<PSColorConstant>>(gfx, colorConstBuffer, 0));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	AddBindable(std::make_unique<InputLayout>(gfx, ied, pvsbc));

	AddBindable(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBindable(std::make_unique<TransformConstBuffer>(gfx, *this));
}

DirectX::XMMATRIX PointLightModel::GetTranformMatrix() const noexcept
{
	return DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}

VOID PointLightModel::SetPosition(DirectX::XMFLOAT3 position)
{
	this->m_position = position;
}