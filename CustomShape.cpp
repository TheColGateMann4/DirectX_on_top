#include "CustomShape.h"
#include "BindableClassesMacro.h"
#include "Application.h"
#include "Includes.h"
#include "ObjLoader.h"

CustomShape::CustomShape(GFX& gfx, std::wstring objpath, std::wstring texturePath)
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			struct
			{
				FLOAT x, y, z;
			} position;
			struct
			{
				FLOAT u, v;
			} texture;
		};

		Mesh<Vertex> CustomMesh = GetCustomMesh<Vertex>(objpath);

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, CustomMesh.m_vertices));

		std::unique_ptr<VertexShader> pVertexShader = std::make_unique<VertexShader>(gfx, L"VertexTextureShader.cso");
		ID3DBlob* pBlob = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));


		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelTextureShader.cso"));

		AddStaticIndexBufferBind(std::make_unique<IndexBuffer>(gfx, CustomMesh.m_indices));

		AddStaticBind(std::make_unique<SamplerState>(gfx, D3D11_TEXTURE_ADDRESS_WRAP)); //D3D11_TEXTURE_ADDRESS_WRAP

		AddStaticBind(std::make_unique<Texture>(gfx, texturePath));


		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
	struct Texture {
		struct {
			FLOAT u, v;
		}position;
		size_t index;
	};
	ObjLoader<V, void, Texture> objLoader(objpath);

	customMesh.m_vertices = objLoader.GetVerticies();
	customMesh.m_indices = objLoader.GetIndicies();
	std::vector<Texture> textures = objLoader.GetTextureCoords();

	for (size_t i = 0; i < textures.size(); i++)
	{
		customMesh.m_vertices[textures[i].index].texture.u = textures[i].position.u;
		customMesh.m_vertices[textures[i].index].texture.v = textures[i].position.v;
	}

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