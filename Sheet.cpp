#include "Sheet.h"
#include "BindableClassesMacro.h"
#include "Application.h"
#include "Includes.h"

Sheet::Sheet(GFX& gfx, const UINT32 TesselationRatio)
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
		Mesh<Vertex> TesselatedSheet = GetTesselatedMesh<Vertex>(TesselationRatio, 1);

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, TesselatedSheet.m_vertices));

		std::unique_ptr<VertexShader> pVertexShader = std::make_unique<VertexShader>(gfx, L"VertexTextureShader.cso");
		ID3DBlob* pBlob = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));


		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelTextureShader.cso"));

		AddStaticIndexBufferBind(std::make_unique<IndexBuffer>(gfx, TesselatedSheet.m_indices));

		AddStaticBind(std::make_unique<SamplerState>(gfx, D3D11_TEXTURE_ADDRESS_WRAP)); //D3D11_TEXTURE_ADDRESS_WRAP

		AddStaticBind(std::make_unique<Texture>(gfx, L"C:\\Users\\U¿ytkownik1\\Desktop\\Shoppy\\movingprimordial.gif"));


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
	AddBindable(std::make_unique<PSConstBuffer>(gfx));
}

template <class V>
Mesh<V> Sheet::GetTesselatedMesh(const UINT32 TesselationRatio, const UINT32 textureRatio)
{
	if (TesselationRatio == 0)
		std::abort();

	Mesh<V> tesselatedMesh;
	const float maxNormalizedPosition = 1.0f;
	const float minNormalizedPosition = 0.0f;
	const float lengthOfTriangle = (maxNormalizedPosition - minNormalizedPosition) / TesselationRatio;

	for (UINT32 row = 0; row < TesselationRatio + 1; row++)
		for (UINT32 column = 0; column < TesselationRatio + 1; column++)
		{
			tesselatedMesh.m_vertices.push_back({ { column * lengthOfTriangle, 0, row * lengthOfTriangle }, {(column * lengthOfTriangle) * textureRatio,( row * lengthOfTriangle) * textureRatio} });

			if (row < TesselationRatio && column < TesselationRatio)
			{
				//front
				tesselatedMesh.m_indices.push_back(((row + 1) * (TesselationRatio + 1)) + column);
				tesselatedMesh.m_indices.push_back(((row + 1) * (TesselationRatio + 1)) + column + 1);
				tesselatedMesh.m_indices.push_back((row * (TesselationRatio + 1)) + column);

				tesselatedMesh.m_indices.push_back(((row + 1) * (TesselationRatio + 1)) + column + 1);
				tesselatedMesh.m_indices.push_back((row * (TesselationRatio + 1)) + column + 1);
				tesselatedMesh.m_indices.push_back((row * (TesselationRatio + 1)) + column);
				
				//back
				tesselatedMesh.m_indices.push_back(((row + 1) * (TesselationRatio + 1)) + column + 1);
				tesselatedMesh.m_indices.push_back(((row + 1) * (TesselationRatio + 1)) + column);
				tesselatedMesh.m_indices.push_back((row * (TesselationRatio + 1)) + column);

				tesselatedMesh.m_indices.push_back(((row + 1) * (TesselationRatio + 1)) + column + 1);
				tesselatedMesh.m_indices.push_back((row * (TesselationRatio + 1)) + column);
				tesselatedMesh.m_indices.push_back((row * (TesselationRatio + 1)) + column + 1);
			}
		}

	return tesselatedMesh;
}

VOID Sheet::Update(FLOAT DeltaTime) noexcept
{
	roll += droll * DeltaTime;
	pitch += dpitch * DeltaTime;
	yaw += dyaw * DeltaTime;

	theta += dtheta * DeltaTime;
	phi += dphi * DeltaTime;
	chi += dchi * DeltaTime;
}

DirectX::XMMATRIX Sheet::GetTranformMatrix() const noexcept
{
	return
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0, 0.0) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0, 0.0, 20.0);
}