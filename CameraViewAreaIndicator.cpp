#include "CameraViewAreaIndicator.h"
#include "Graphics.h"
#include "Vertex.h"
#include "BindableClassesMacro.h"
#include "Camera.h"

CameraViewAreaIndicator::CameraViewAreaIndicator(GFX& gfx, Camera* parent)
	:
	m_parent(parent)
{
	m_ignoreVisibility = true;

	CameraSettings cameraSettings;
	m_parent->GetCameraSettings(&cameraSettings);

	float startLength = CalculateLengthOfViewTriangle(cameraSettings.m_Fov, cameraSettings.m_NearZ);
	float endLength = CalculateLengthOfViewTriangle(cameraSettings.m_Fov, cameraSettings.m_FarZ);


	std::vector<D3D11_INPUT_ELEMENT_DESC> DirectXLayout;
	std::vector<UINT32> indices;

	m_pTransformConstBuffer = TransformConstBuffer::GetBindable(gfx, *this, { {TargetVertexShader, 0} });
	m_pVertexBuffer = GetVertexBuffer(gfx, startLength, endLength, &cameraSettings, m_vertices, &DirectXLayout);
	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, "$cameraIndicator", indices);
	m_pTopology = Topology::GetBindable(gfx, D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	{
		RenderTechnique technique;

		{
			RenderStep step("normalPass");

			DynamicConstantBuffer::BufferLayout constBufferLayout;
			constBufferLayout.Add<DynamicConstantBuffer::DataType::Float4>("color");

			DynamicConstantBuffer::BufferData constBufferData(std::move(constBufferLayout));
			*constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("color") = DirectX::XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS.cso");

			step.AddBindable(InputLayout::GetBindable(gfx, DirectXLayout, pVertexShader.get()));
			step.AddBindable(pVertexShader);
			step.AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));
			step.AddBindable(CachedBuffer::GetBindable(gfx, constBufferData, {{TargetPixelShader, 1}}));

			technique.AddRenderStep(step);
		}

		{
			RenderStep step("ignoreZBufferPass");

			DynamicConstantBuffer::BufferLayout constBufferLayout;
			constBufferLayout.Add<DynamicConstantBuffer::DataType::Float4>("color");

			DynamicConstantBuffer::BufferData constBufferData(std::move(constBufferLayout));
			*constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("color") = DirectX::XMFLOAT4(1.0f, 0.5f, 0.0f, 0.2f);

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS.cso");

			step.AddBindable(InputLayout::GetBindable(gfx, DirectXLayout, pVertexShader.get()));
			step.AddBindable(pVertexShader);
			step.AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));
			step.AddBindable(CachedBuffer::GetBindable(gfx, constBufferData, {{TargetPixelShader, 1}}));

			technique.AddRenderStep(step);
		}

		AddRenderTechnique(technique);
	}
}

DirectX::XMMATRIX CameraViewAreaIndicator::GetTranformMatrix() const noexcept
{
	return m_parent->GetSceneTranformMatrix();
}

std::shared_ptr<VertexBuffer> CameraViewAreaIndicator::GetVertexBuffer(GFX& gfx, float startLength, float endLength, CameraSettings* cameraSettings, std::vector<DirectX::XMFLOAT3>& vertices, std::vector<D3D11_INPUT_ELEMENT_DESC>* layout)
{
	vertices.clear();

	vertices =
	{
		{ startLength * cameraSettings->m_AspectRatio, startLength, cameraSettings->m_NearZ },
		{ startLength * cameraSettings->m_AspectRatio, -startLength, cameraSettings->m_NearZ},
		{ -startLength * cameraSettings->m_AspectRatio, -startLength, cameraSettings->m_NearZ},
		{ -startLength * cameraSettings->m_AspectRatio, startLength, cameraSettings->m_NearZ },

		{ endLength * cameraSettings->m_AspectRatio, endLength, cameraSettings->m_FarZ },
		{ endLength * cameraSettings->m_AspectRatio, -endLength, cameraSettings->m_FarZ },
		{ -endLength * cameraSettings->m_AspectRatio, -endLength, cameraSettings->m_FarZ },
		{ -endLength * cameraSettings->m_AspectRatio, endLength, cameraSettings->m_FarZ }
	};

	DynamicVertex::VertexLayout vertexLayout = DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D);
	DynamicVertex::VertexBuffer vertexBuffer(std::move(vertexLayout));

	for (const auto& position : vertices)
		vertexBuffer.Emplace_Back(position);

	if(layout != nullptr)
		*layout = vertexBuffer.GetLayout().GetDirectXLayout();

	std::string vertexBufferTag = "$cameraViewIndicator@" + std::to_string(cameraSettings->m_Fov) + '@' + std::to_string(cameraSettings->m_FarZ) + '@' + std::to_string(cameraSettings->m_NearZ) + '@' + std::to_string(cameraSettings->m_AspectRatio);

	return VertexBuffer::GetBindable(gfx, vertexBufferTag.c_str(), vertexBuffer);
}

float CameraViewAreaIndicator::CalculateLengthOfViewTriangle(float fov, float width)
{
	float C = fov / 2;
	float B = _Pi / 2;
	float A = _Pi - (C + B);

	float a = width;
	float b = a * std::sin(B) / std::sin(A);

	float Area = (a * b * std::sin(C)) / 2;

	return (2 * Area) / a;
}

void CameraViewAreaIndicator::UpdateVertexBuffer(GFX& gfx)
{
	CameraSettings cameraSettings = {};

	m_parent->GetCameraSettings(&cameraSettings);

	float startLength = CalculateLengthOfViewTriangle(cameraSettings.m_Fov, cameraSettings.m_NearZ);
	float endLength = CalculateLengthOfViewTriangle(cameraSettings.m_Fov, cameraSettings.m_FarZ);

	m_pVertexBuffer = GetVertexBuffer(gfx, startLength, endLength, &cameraSettings, m_vertices);
}

const DirectX::XMFLOAT3* CameraViewAreaIndicator::GetVerticeBuffer() const
{
	return &m_vertices.at(0);
}

UINT32 CameraViewAreaIndicator::GetVerticeBufferByteSize() const
{
	return m_vertices.size() * sizeof(DirectX::XMFLOAT3);
}