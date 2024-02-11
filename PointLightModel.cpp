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

	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, "3535SPHERE", model.m_indices);
	m_pVertexBuffer = VertexBuffer::GetBindable(gfx, "3535SPHERE", model.m_vertices);
	m_pTopology = Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pTransformConstBuffer = std::make_shared<TransformConstBufferWithPixelShader>(gfx, *this, 0, 2);

	{
		RenderTechnique normalTechnique;

		{
			RenderSteps normalStep(PASS_NORMAL);

			std::shared_ptr<VertexShader> vertexShader = VertexShader::GetBindable(gfx, "VS.cso");
			ID3DBlob* pBlob = vertexShader->GetByteCode();

			*m_colorBuffer.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("element0") = { 1.0f, 1.0f, 1.0f };


			normalStep.AddBindable(std::move(vertexShader));

			normalStep.AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));

			normalStep.AddBindable(std::make_shared<CachedBuffer>(gfx, m_colorBuffer, 1, true));

			normalStep.AddBindable(InputLayout::GetBindable(gfx, model.GetLayout(), pBlob));

			normalTechnique.AddRenderStep(normalStep);
		}

		AddRenderTechnique(normalTechnique);
	}
}

DirectX::XMMATRIX PointLightModel::GetTranformMatrix() const noexcept
{
	return DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}

VOID PointLightModel::SetPosition(const DirectX::XMFLOAT3& position)
{
	this->m_position = position;
}

void PointLightModel::UpdateLightColorBuffer(GFX& gfx, const DirectX::XMFLOAT3& color)
{
	*m_colorBuffer.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("element0") = color; // should be named color, but since we make layout by identificator string we don't have normal name
 	CachedBuffer* pPixelConstBuffer = GetBindable<CachedBuffer>( 0, 0, 1 );
 	pPixelConstBuffer->Update(gfx, m_colorBuffer);
}