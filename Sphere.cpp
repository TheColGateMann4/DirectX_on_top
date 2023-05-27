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


	auto mesh = Sphere::GetMesh<Vertex>(1.0f, (UINT32)latdist(rng), (UINT32)longdist(rng));

	mesh.Transform(DirectX::XMMatrixScaling(1.0, 1.0, 1.2));


	AddBindable(std::make_unique<VertexBuffer>(gfx, mesh.m_vertices));

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, mesh.m_indices));

	AddBindable(std::make_unique<TransformConstBuffer>(gfx, *this));
}

template <class T>
static Mesh<T> Sphere::GetMesh( FLOAT Radius, UINT32 HorizontalDivisions, UINT32 VerticalDivisions )
{
	const DirectX::XMVECTOR startPosition = DirectX::XMVectorSet(0, 0, Radius, 0);
	const FLOAT lattitudeAngle = std::_Pi / HorizontalDivisions;
	const FLOAT longitudeAngle = 2 * std::_Pi / VerticalDivisions;

	std::vector<T> vertices;
	for (UINT32 iVerticalDivisions = 1; iVerticalDivisions < VerticalDivisions; iVerticalDivisions++)
	{
		const DirectX::XMVECTOR currVerticalPos = DirectX::XMVector3Transform(startPosition, DirectX::XMMatrixRotationX(lattitudeAngle * iVerticalDivisions));
		for (UINT32 iHorizontalDivision = 0; iHorizontalDivision < HorizontalDivisions; iHorizontalDivision++)
		{
			vertices.emplace_back();
			DirectX::XMVECTOR verticePos = DirectX::XMVector3Transform(currVerticalPos, DirectX::XMMatrixRotationZ(longitudeAngle * HorizontalDivisions));
			DirectX::XMStoreFloat3(&vertices.back().position, verticePos);
		}
	}

	//cap
	const auto iNorthPole = vertices.size();
	vertices.emplace_back();
	DirectX::XMStoreFloat3(&vertices.back().position, startPosition);

	const auto iSouthPole = vertices.size();
	vertices.emplace_back();
	DirectX::XMStoreFloat3(&vertices.back().position, DirectX::XMVectorNegate(startPosition));

	const auto ResolveIndex = [VerticalDivisions, HorizontalDivisions](UINT32 iVerticalDivisions, UINT32 iHorizontalDivision)
	{return iVerticalDivisions * VerticalDivisions + iHorizontalDivision;};

	std::vector<UINT32> indices;
	for (UINT32 iVerticalDivisions = 0; iVerticalDivisions < VerticalDivisions - 2; iVerticalDivisions++)
	{
		for (UINT32 iHorizontalDivision = 0; iHorizontalDivision < HorizontalDivisions - 1; iHorizontalDivision++)
		{
			indices.push_back(ResolveIndex(iVerticalDivisions, iHorizontalDivision));
			indices.push_back(ResolveIndex(iVerticalDivisions + 1, iHorizontalDivision));
			indices.push_back(ResolveIndex(iVerticalDivisions, iHorizontalDivision + 1));
			indices.push_back(ResolveIndex(iVerticalDivisions, iHorizontalDivision + 1));
			indices.push_back(ResolveIndex(iVerticalDivisions + 1, iHorizontalDivision));
			indices.push_back(ResolveIndex(iVerticalDivisions + 1, iHorizontalDivision + 1));
		}

		//connecting two ends on longitude
		indices.push_back(ResolveIndex(iVerticalDivisions, HorizontalDivisions - 1));
		indices.push_back(ResolveIndex(iVerticalDivisions + 1, HorizontalDivisions - 1));
		indices.push_back(ResolveIndex(iVerticalDivisions, 0));
		indices.push_back(ResolveIndex(iVerticalDivisions, 0));
		indices.push_back(ResolveIndex(iVerticalDivisions + 1, HorizontalDivisions - 1));
		indices.push_back(ResolveIndex(iVerticalDivisions + 1, 0));
	}

	//making poles
	for (UINT32 iHorizontalDivision = 0; iHorizontalDivision < HorizontalDivisions - 1; iHorizontalDivision++)
	{
		//north
		indices.push_back(iNorthPole);
		indices.push_back(ResolveIndex(0, iHorizontalDivision));
		indices.push_back(ResolveIndex(0, iHorizontalDivision + 1));

		//south
		indices.push_back(ResolveIndex(VerticalDivisions - 2, iHorizontalDivision + 1));
		indices.push_back(ResolveIndex(VerticalDivisions - 2, iHorizontalDivision));
		indices.push_back(iSouthPole);
	}

	//north
	indices.push_back(iNorthPole);
	indices.push_back(ResolveIndex(0, HorizontalDivisions - 1));
	indices.push_back(ResolveIndex(0, 0));

	//south
	indices.push_back(ResolveIndex(VerticalDivisions - 2, 0));
	indices.push_back(ResolveIndex(VerticalDivisions - 2, HorizontalDivisions - 1));
	indices.push_back(iSouthPole);

	return { std::move(vertices), std::move(indices) };
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