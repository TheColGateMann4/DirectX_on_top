#include "PointLightModel.h"
#include "BindableClassesMacro.h"
#include "SimpleMesh.h"
#include "Sphere.h"

PointLightModel::PointLightModel(GFX& gfx, float radius)
	:
	m_colorBuffer(DynamicConstantBuffer::BufferLayout("F3"))
{
	SimpleMesh model = Sphere::GetMesh(35, 35);
	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	AddBindable(VertexBuffer::GetBindable(gfx, "3535SPHERE", model.m_vertices));
	AddIndexBuffer(IndexBuffer::GetBindable(gfx, "3535SPHERE", model.m_indices));

	auto pvs = VertexShader::GetBindable(gfx, "VS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBindable(std::move(pvs));

	AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));

	AddBindable(std::make_shared<CachedBuffer>(gfx, m_colorBuffer, 1, true));

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
	*m_colorBuffer.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("element0") = color; // should be named color, but since we make layout by identificator string we don't have normal name
	CachedBuffer* pPixelConstBuffer = GetBindable<CachedBuffer>();
	pPixelConstBuffer->Update(gfx, m_colorBuffer);
}