#include "PointLightModel.h"
#include "BindableClassesMacro.h"
#include "SimpleMesh.h"
#include "Sphere.h"
#include "PointLight.h"



PointLightModel::PointLightModel(GFX& gfx, PointLight* parent, float radius)
	:
	m_colorBuffer(DynamicConstantBuffer::BufferLayout("F4")),
	m_parent(parent)
{
	SimpleMesh model = Sphere::GetMesh(1.0f, 35, 35);
	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, "3535SPHERE", model.m_indices);
	m_pVertexBuffer = VertexBuffer::GetBindable(gfx, "3535SPHERE", model.m_vertices);
	m_pTopology = Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pTransformConstBuffer = std::make_shared<TransformConstBufferWithPixelShader>(gfx, *this, 0, 2);

	{
		RenderTechnique normalTechnique("normal");

		{
			RenderStep normalStep("normalPass");

			std::shared_ptr<VertexShader> vertexShader = VertexShader::GetBindable(gfx, "VS.cso");

			*m_colorBuffer.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("element0") = { 1.0f, 1.0f, 1.0f, 1.0f };


			normalStep.AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));

			normalStep.AddBindable(std::make_shared<CachedBuffer>(gfx, m_colorBuffer, 1, TargetPixelShader));

			normalStep.AddBindable(InputLayout::GetBindable(gfx, model.GetLayout(), vertexShader.get()));

			normalStep.AddBindable(RasterizerState::GetBindable(gfx, true));

			normalStep.AddBindable(std::move(vertexShader));

			normalTechnique.AddRenderStep(normalStep);
		}

		AddRenderTechnique(normalTechnique);
	}
}

DirectX::XMMATRIX PointLightModel::GetTranformMatrix() const noexcept
{
	return m_parent->GetSceneTranformMatrix();
}

void PointLightModel::UpdateLightColorBuffer(GFX& gfx, const DirectX::XMFLOAT3& color)
{
	*m_colorBuffer.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("element0") = color; // should be named color, but since we make layout by identificator string we don't have normal name
 	CachedBuffer* pPixelConstBuffer = GetBindable<CachedBuffer>( 0, 0, 1 );
 	pPixelConstBuffer->Update(gfx, m_colorBuffer);
}