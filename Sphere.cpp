#include "Sphere.h"
#include "BindableClassesMacro.h"

Sphere::Sphere(GFX& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<UINT32>& longdist,
	std::uniform_int_distribution<UINT32>& latdist)
	:
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	if (!IsStaticInitialized())
	{
		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
		auto pBlob = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

		struct ConstantBufferColor
		{
			struct
			{
				float r, g, b, a;
			}face_colors[8];
		};

		const ConstantBufferColor constbufferColor
		{
			{
				{ 1.0f,1.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f },
				{ 0.0f,1.0f,1.0f },
				{ 0.0f,0.0f,0.0f },
			}
		};
		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBufferColor>>(gfx, constbufferColor, 0));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pBlob));

		AddStaticBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};


	Mesh<Vertex> mesh = GetMesh<Vertex>(latdist(rng), longdist(rng));

	mesh.Transform(DirectX::XMMatrixScaling(1.0, 1.0, 1.2));


	AddBindable(std::make_unique<VertexBuffer>(gfx, mesh.m_vertices));

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, mesh.m_indices));

	AddBindable(std::make_unique<TransformConstBuffer>(gfx, *this));
}

VOID Sphere::Update(FLOAT DeltaTime) noexcept
{
	pitch += dpitch * DeltaTime;
	yaw += dyaw * DeltaTime;
	roll += droll * DeltaTime;

	theta += dtheta * DeltaTime;
	phi += dphi * DeltaTime;
	chi += dchi * DeltaTime;
}

DirectX::XMMATRIX Sphere::GetTranformMatrix() const noexcept
{
	return
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0, 0.0) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0, 0.0, 20.0);
}