#include "Sphere.h"
#include "BindableClassesMacro.h"

Sphere::Sphere(GFX& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<int>& longdist,
	std::uniform_int_distribution<int>& latdist)
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
		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBufferColor>>(gfx, constbufferColor));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pBlob));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};


	auto mesh = Sphere::GetMesh<Vertex>((UINT32)latdist(rng), (UINT32)longdist(rng));

	mesh.Transform(DirectX::XMMatrixScaling(1.0, 1.0, 1.2));


	AddBindable(std::make_unique<VertexBuffer>(gfx, mesh.m_vertices));

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, mesh.m_indices));

	AddBindable(std::make_unique<TransformConstBuffer>(gfx, *this));
}

template <class T>
static Mesh<T> Sphere::GetMesh(UINT32 latDiv, UINT32 longDiv)
{
	namespace dx = DirectX;
	assert(latDiv >= 3);
	assert(longDiv >= 3);

	constexpr float radius = 1.0f;
	const auto base = dx::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
	const float lattitudeAngle = std::_Pi / latDiv;
	const float longitudeAngle = 2.0f * std::_Pi / longDiv;

	std::vector<T> vertices;
	for (int iLat = 1; iLat < latDiv; iLat++)
	{
		const auto latBase = dx::XMVector3Transform(
			base,
			dx::XMMatrixRotationX(lattitudeAngle * iLat)
		);
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			auto v = dx::XMVector3Transform(
				latBase,
				dx::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			dx::XMStoreFloat3(&vertices.back().position, v);
		}
	}

	// add the cap vertices
	const auto iNorthPole = (UINT32)vertices.size();
	vertices.emplace_back();
	dx::XMStoreFloat3(&vertices.back().position, base);
	const auto iSouthPole = (UINT32)vertices.size();
	vertices.emplace_back();
	dx::XMStoreFloat3(&vertices.back().position, dx::XMVectorNegate(base));

	const auto calcIdx = [latDiv, longDiv](UINT32 iLat, UINT32 iLong)
	{ return iLat * longDiv + iLong; };
	std::vector<UINT32> indices;
	for (UINT32 iLat = 0; iLat < latDiv - 2; iLat++)
	{
		for (UINT32 iLong = 0; iLong < longDiv - 1; iLong++)
		{
			indices.push_back(calcIdx(iLat, iLong));
			indices.push_back(calcIdx(iLat + 1, iLong));
			indices.push_back(calcIdx(iLat, iLong + 1));
			indices.push_back(calcIdx(iLat, iLong + 1));
			indices.push_back(calcIdx(iLat + 1, iLong));
			indices.push_back(calcIdx(iLat + 1, iLong + 1));
		}
		// wrap band
		indices.push_back(calcIdx(iLat, longDiv - 1));
		indices.push_back(calcIdx(iLat + 1, longDiv - 1));
		indices.push_back(calcIdx(iLat, 0));
		indices.push_back(calcIdx(iLat, 0));
		indices.push_back(calcIdx(iLat + 1, longDiv - 1));
		indices.push_back(calcIdx(iLat + 1, 0));
	}

	// cap fans
	for (UINT32 iLong = 0; iLong < longDiv - 1; iLong++)
	{
		// north
		indices.push_back(iNorthPole);
		indices.push_back(calcIdx(0, iLong));
		indices.push_back(calcIdx(0, iLong + 1));
		// south
		indices.push_back(calcIdx(latDiv - 2, iLong + 1));
		indices.push_back(calcIdx(latDiv - 2, iLong));
		indices.push_back(iSouthPole);
	}
	// wrap triangles
	// north
	indices.push_back(iNorthPole);
	indices.push_back(calcIdx(0, longDiv - 1));
	indices.push_back(calcIdx(0, 0));
	// south
	indices.push_back(calcIdx(latDiv - 2, 0));
	indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
	indices.push_back(iSouthPole);

	return { std::move(vertices),std::move(indices) };
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