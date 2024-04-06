#include "CameraViewAreaIndicator.h"
#include "Graphics.h"
#include "Vertex.h"
#include "BindableClassesMacro.h"
#include "Camera.h"

CameraViewAreaIndicator::CameraViewAreaIndicator(GFX& gfx, Camera* parent)
	:
	m_parent(parent)
{
	float aspectRatio = parent->m_AspectRatio;

	float fov = m_parent->m_Fov;
	float nearZ = m_parent->m_NearZ;
	float farZ = m_parent->m_FarZ;
	float startLength = CalculateLengthOfViewTriangle(fov, nearZ);
	float endLength = CalculateLengthOfViewTriangle(fov, farZ);


	std::vector<D3D11_INPUT_ELEMENT_DESC> DirectXLayout;
	std::vector<UINT32> indices;

	m_pTransformConstBuffer = std::make_shared<TransformConstBuffer>(gfx, *this, 0);
	m_pVertexBuffer = GetVertexBuffer(gfx, fov, nearZ, farZ, startLength, endLength, aspectRatio, &DirectXLayout);
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
			step.AddBindable(std::make_shared<CachedBuffer>(gfx, constBufferData, 1, true));

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
			step.AddBindable(std::make_shared<CachedBuffer>(gfx, constBufferData, 1, true));

			technique.AddRenderStep(step);
		}

		AddRenderTechnique(technique);
	}
}

DirectX::XMMATRIX CameraViewAreaIndicator::GetTranformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_parent->m_rotation.y, m_parent->m_rotation.x, m_parent->m_rotation.z) *
		DirectX::XMMatrixTranslation(m_parent->m_position.x, m_parent->m_position.y, m_parent->m_position.z);
}

std::shared_ptr<VertexBuffer> CameraViewAreaIndicator::GetVertexBuffer(GFX& gfx, float fov, float nearZ, float farZ, float startLength, float endLength, float aspectRatio, std::vector<D3D11_INPUT_ELEMENT_DESC>* layout)
{
	std::vector<DirectX::XMFLOAT3> vertices
	{
		{ startLength * aspectRatio, startLength, nearZ },
		{ startLength * aspectRatio, -startLength, nearZ},
		{ -startLength * aspectRatio, -startLength, nearZ},
		{ -startLength * aspectRatio, startLength, nearZ },

		{ endLength * aspectRatio, endLength, farZ },
		{ endLength * aspectRatio, -endLength, farZ },
		{ -endLength * aspectRatio, -endLength, farZ },
		{ -endLength * aspectRatio, endLength, farZ }
	};

	DynamicVertex::VertexLayout vertexLayout = DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D);
	DynamicVertex::VertexBuffer vertexBuffer(std::move(vertexLayout));

	for (const auto& position : vertices)
		vertexBuffer.Emplace_Back(position);

	if(layout != nullptr)
		*layout = vertexBuffer.GetLayout().GetDirectXLayout();

	std::string vertexBufferTag = "$cameraViewIndicator@" + std::to_string(fov) + '@' + std::to_string(farZ) + '@' + std::to_string(nearZ);

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
	float aspectRatio = m_parent->m_AspectRatio;

	float fov = m_parent->m_Fov;
	float nearZ = m_parent->m_NearZ;
	float farZ = m_parent->m_FarZ;
	float startLength = CalculateLengthOfViewTriangle(fov, nearZ);
	float endLength = CalculateLengthOfViewTriangle(fov, farZ);

	m_pVertexBuffer = GetVertexBuffer(gfx, fov, nearZ, farZ, startLength, endLength, aspectRatio, nullptr);
}