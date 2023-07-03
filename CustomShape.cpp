#include "CustomShape.h"
#include "BindableClassesMacro.h"
#include "Application.h"
#include "Includes.h"
#include "ObjLoader.h"

CustomShape::CustomShape(GFX& gfx, std::wstring objpath)
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			struct
			{
				FLOAT x, y, z;
			} position;
		};
		Mesh<Vertex> CustomMesh = GetCustomMesh<Vertex>(objpath);

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, CustomMesh.m_vertices));

		std::unique_ptr<VertexShader> pVertexShader = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
		ID3DBlob* pBlob = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));


		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

		AddStaticIndexBufferBind(std::make_unique<IndexBuffer>(gfx, CustomMesh.m_indices));

		AddStaticBind(std::make_unique<SamplerState>(gfx, D3D11_TEXTURE_ADDRESS_WRAP)); //D3D11_TEXTURE_ADDRESS_WRAP


		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pBlob));

		AddStaticBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		GetIndexBufferFromVector();
	}
	AddBindable(std::make_unique<TransformConstBuffer>(gfx, *this));
}

template <class V>
Mesh<V> CustomShape::GetCustomMesh(std::wstring& objpath)
{
	Mesh<V> customMesh;
	ObjLoader<V> objLoader(objpath, false);

	customMesh.m_vertices = objLoader.GetVerticies();
	customMesh.m_indices = objLoader.GetIndicies();

	return customMesh;
}

VOID CustomShape::Update(FLOAT DeltaTime) noexcept
{
	roll += droll * DeltaTime;
	pitch += dpitch * DeltaTime;
	yaw += dyaw * DeltaTime;

	theta += dtheta * DeltaTime;
	phi += dphi * DeltaTime;
	chi += dchi * DeltaTime;
}

DirectX::XMMATRIX CustomShape::GetTranformMatrix() const noexcept
{
	return
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0, 0.0) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}