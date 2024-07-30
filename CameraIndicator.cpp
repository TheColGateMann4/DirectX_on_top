#include "CameraIndicator.h"
#include "Graphics.h"
#include "Vertex.h"
#include "BindableClassesMacro.h"
#include "Camera.h"

CameraIndicator::CameraIndicator(GFX& gfx, Camera* parent)
	:
	m_parent(parent)
{
	m_ignoreVisibility = true;

	CameraSettings cameraSettings;
	m_parent->GetCameraSettings(&cameraSettings);

	std::vector<UINT32> indices =
	{
		//small square
		0,1,	0,3,
		1,2,	2,3,

		//big square
		4,5,	4,7,
		5,6,	6,7,

		//linking between both
		0,4,	1,5,
		2,6,	3,7
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> layout;

	m_pTransformConstBuffer = TransformConstBuffer::GetBindable(gfx, *this, { {TargetVertexShader, 0} });
	m_pVertexBuffer = GetVertexBuffer(gfx, cameraSettings.m_AspectRatio, &layout);
	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, "$cameraIndicator", indices);
	m_pTopology = Topology::GetBindable(gfx, D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	RenderTechnique technique;
	{
		RenderStep step("normalPass");


		DynamicConstantBuffer::BufferLayout constBufferLayout;
		constBufferLayout.Add<DynamicConstantBuffer::DataType::Float4>("color");

		DynamicConstantBuffer::BufferData constBufferData(std::move(constBufferLayout));
		*constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("color") = DirectX::XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);


		std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS.cso");

		step.AddBindable(InputLayout::GetBindable(gfx, layout, pVertexShader.get()));
		step.AddBindable(pVertexShader);
		step.AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));
		step.AddBindable(CachedBuffer::GetBindable(gfx, constBufferData, {{TargetPixelShader, 1}}));

		technique.AddRenderStep(step);
	}

	AddRenderTechnique(technique);
}

DirectX::XMMATRIX CameraIndicator::GetTranformMatrix() const noexcept
{
	return m_parent->GetSceneTranformMatrix();
}

void CameraIndicator::UpdateVertexBuffer(GFX& gfx)
{
	CameraSettings cameraSettings;
	m_parent->GetCameraSettings(&cameraSettings);

	m_pVertexBuffer = GetVertexBuffer(gfx, cameraSettings.m_AspectRatio);
}

std::shared_ptr<VertexBuffer> CameraIndicator::GetVertexBuffer(GFX& gfx, float aspectRatio, std::vector<D3D11_INPUT_ELEMENT_DESC>* layout)
{
	std::vector<DirectX::XMFLOAT3> vertices
	{
		{ 0.2f, 0.2f, -1.0f },
		{ 0.2f, -0.2f, -1.0f },
		{ -0.2f, -0.2f, -1.0f },
		{ -0.2f, 0.2f, -1.0f },

		{ 0.7f * aspectRatio, 0.7f, 0.0f },
		{ 0.7f * aspectRatio, -0.7f, 0.0f },
		{ -0.7f * aspectRatio, -0.7f, 0.0f },
		{ -0.7f * aspectRatio, 0.7f, 0.0f }
	};

	DynamicVertex::VertexLayout vertexLayout = DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D);
	DynamicVertex::VertexBuffer vertexBuffer(std::move(vertexLayout));

	for (const auto& position : vertices)
		vertexBuffer.Emplace_Back(position);

	if (layout != nullptr)
		*layout = vertexBuffer.GetLayout().GetDirectXLayout();

	std::string vertexBufferTag = "$cameraIndicator@" + std::to_string(aspectRatio);

	return VertexBuffer::GetBindable(gfx, vertexBufferTag.c_str(), vertexBuffer);
}