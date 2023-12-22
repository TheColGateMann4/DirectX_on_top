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

	std::vector<std::shared_ptr<Bindable>> meshBindables = {};

	meshBindables.push_back(VertexBuffer::GetBindable(gfx, "3535SPHERE", model.m_vertices));
	meshBindables.push_back(IndexBuffer::GetBindable(gfx, "3535SPHERE", model.m_indices));

	auto pvs = VertexShader::GetBindable(gfx, "VS.cso");
	auto pvsbc = pvs->GetByteCode();
	meshBindables.push_back(std::move(pvs));

	meshBindables.push_back(PixelShader::GetBindable(gfx, "PS_Solid.cso"));

	*m_colorBuffer.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("element0") = { 1.0f, 1.0f, 1.0f };

	meshBindables.push_back(std::make_shared<CachedBuffer>(gfx, m_colorBuffer, 1, true));

	meshBindables.push_back(InputLayout::GetBindable(gfx, model.GetLayout(), pvsbc));

	meshBindables.push_back(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	m_mesh = std::make_unique<Mesh>(gfx, meshBindables);
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
	CachedBuffer* pPixelConstBuffer = m_mesh->GetBindable<CachedBuffer>();
	pPixelConstBuffer->Update(gfx, m_colorBuffer);
}