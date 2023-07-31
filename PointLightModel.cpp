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
	SimpleMesh model = Sphere::GetMesh(35, 35);
	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	AddBindable(VertexBuffer::GetBindable(gfx, "3535SPHERE", model.m_vertices));
	AddIndexBuffer(IndexBuffer::GetBindable(gfx, "3535SPHERE", model.m_indices));

	auto pvs = VertexShader::GetBindable(gfx, "VertexShader.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBindable(std::move(pvs));

	AddBindable(PixelShader::GetBindable(gfx, "PixelSolidColorShader.cso"));

	AddBindable(PixelConstantBuffer<PSColorConstant>::GetBindable(gfx, m_color, 0));

	AddBindable(InputLayout::GetBindable(gfx, model.GetLayout(), pvsbc));

	AddBindable(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

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

void PointLightModel::UpdateLightColorBuffer(GFX& gfx, DirectX::XMFLOAT3 color)
{
	m_color.color = color;
	PixelConstantBuffer<PSColorConstant>* pPixelConstBuffer = GetBindable<PixelConstantBuffer<PSColorConstant>>();
	pPixelConstBuffer->Update(gfx, m_color);
}