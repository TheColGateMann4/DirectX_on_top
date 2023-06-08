// #include "Sphere.h"
// #include "BindableClassesMacro.h"
// 
// Sphere::Sphere(GFX& gfx, std::mt19937& rng,
// 	std::uniform_real_distribution<float>& adist,
// 	std::uniform_real_distribution<float>& ddist,
// 	std::uniform_real_distribution<float>& odist,
// 	std::uniform_real_distribution<float>& rdist,
// 	std::uniform_int_distribution<int>& longdist,
// 	std::uniform_int_distribution<int>& latdist)
// 	:
// 	r(rdist(rng)),
// 	droll(ddist(rng)),
// 	dpitch(ddist(rng)),
// 	dyaw(ddist(rng)),
// 	dphi(odist(rng)),
// 	dtheta(odist(rng)),
// 	dchi(odist(rng)),
// 	chi(adist(rng)),
// 	theta(adist(rng)),
// 	phi(adist(rng))
// {
// 	if (!IsStaticInitialized())
// 	{
// 		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
// 		auto pBlob = pVertexShader->GetByteCode();
// 		AddStaticBind(std::move(pVertexShader));
// 
// 		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));
// 
// 		struct ConstantBufferColor
// 		{
// 			struct
// 			{
// 				float r, g, b, a;
// 			}face_colors[8];
// 		};
// 
// 		const ConstantBufferColor constbufferColor
// 		{
// 			{
// 				{ 1.0f,1.0f,1.0f },
// 				{ 1.0f,0.0f,0.0f },
// 				{ 0.0f,1.0f,0.0f },
// 				{ 1.0f,1.0f,0.0f },
// 				{ 0.0f,0.0f,1.0f },
// 				{ 1.0f,0.0f,1.0f },
// 				{ 0.0f,1.0f,1.0f },
// 				{ 0.0f,0.0f,0.0f },
// 			}
// 		};
// 		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBufferColor>>(gfx, constbufferColor));
// 
// 		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
// 		{
// 			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
// 		};
// 
// 		AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pBlob));
// 
// 		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
// 	}
// 
// 	struct Vertex
// 	{
// 		DirectX::XMFLOAT3 position;
// 	};
// 
// 
// 	auto mesh = Sphere::GetMesh<Vertex>((UINT32)latdist(rng), (UINT32)longdist(rng));
// 
// 	mesh.Transform(DirectX::XMMatrixScaling(1.0, 1.0, 1.2));
// 
// 
// 	AddBindable(std::make_unique<VertexBuffer>(gfx, mesh.m_vertices));
// 
// 	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, mesh.m_indices));
// 
// 	AddBindable(std::make_unique<TransformConstBuffer>(gfx, *this));
// }
// 
// template <class T>
// static Mesh<T> Sphere::GetMesh(UINT32 verticalDivisions, UINT32 horizontalDivisions)
// {
// 	assert(verticalDivisions >= 3);
// 	assert(horizontalDivisions >= 3);
// 
// 	constexpr float radius = 1.0f;
// 	const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
// 	const float verticalAngle = std::_Pi / verticalDivisions;
// 	const float horizontalAngle = 2.0f * std::_Pi / horizontalDivisions;
// 
// 	std::vector<T> vertices;
// 	for (int currVerticalDivisions = 1; currVerticalDivisions < verticalDivisions; currVerticalDivisions++)
// 	{
// 		const auto verticalBase = DirectX::XMVector3Transform(
// 			base,
// 			DirectX::XMMatrixRotationX(verticalAngle * currVerticalDivisions)
// 		);
// 		for (int currHorizontalDivision = 0; currHorizontalDivision < horizontalDivisions; currHorizontalDivision++)
// 		{
// 			vertices.emplace_back();
// 			auto v = DirectX::XMVector3Transform(
// 				verticalBase,
// 				DirectX::XMMatrixRotationZ(horizontalAngle * currHorizontalDivision)
// 			);
// 			DirectX::XMStoreFloat3(&vertices.back().position, v);
// 		}
// 	}
// 
// 	// add the cap vertices
// 	const auto iNorthPole = (UINT32)vertices.size();
// 	vertices.emplace_back();
// 	DirectX::XMStoreFloat3(&vertices.back().position, base);
// 	const auto iSouthPole = (UINT32)vertices.size();
// 	vertices.emplace_back();
// 	DirectX::XMStoreFloat3(&vertices.back().position, DirectX::XMVectorNegate(base));
// 
// 	const auto ResolveIndex = [horizontalDivisions](UINT32 currVerticalDivision, UINT32 currHorizontalDivision)
// 	{ return currVerticalDivision * horizontalDivisions + currHorizontalDivision; };
// 
// 	std::vector<UINT32> indices;
// 	for (UINT32 currVerticalDivision = 0; currVerticalDivision < verticalDivisions - 2; currVerticalDivision++)
// 	{
// 		for (UINT32 currHorizontalDivision = 0; currHorizontalDivision < horizontalDivisions - 1; currHorizontalDivision++)
// 		{
// 			indices.push_back(ResolveIndex(currVerticalDivision, currHorizontalDivision));
// 			indices.push_back(ResolveIndex(currVerticalDivision + 1, currHorizontalDivision));
// 			indices.push_back(ResolveIndex(currVerticalDivision, currHorizontalDivision + 1));
// 			indices.push_back(ResolveIndex(currVerticalDivision, currHorizontalDivision + 1));
// 			indices.push_back(ResolveIndex(currVerticalDivision + 1, currHorizontalDivision));
// 			indices.push_back(ResolveIndex(currVerticalDivision + 1, currHorizontalDivision + 1));
// 		}
// 		// wrap band
// 		indices.push_back(ResolveIndex(currVerticalDivision, horizontalDivisions - 1));
// 		indices.push_back(ResolveIndex(currVerticalDivision + 1, horizontalDivisions - 1));
// 		indices.push_back(ResolveIndex(currVerticalDivision, 0));
// 		indices.push_back(ResolveIndex(currVerticalDivision, 0));
// 		indices.push_back(ResolveIndex(currVerticalDivision + 1, horizontalDivisions - 1));
// 		indices.push_back(ResolveIndex(currVerticalDivision + 1, 0));
// 	}
// 
// 	// cap fans
// 	for (UINT32 currHorizontalDivision = 0; currHorizontalDivision < horizontalDivisions - 1; currHorizontalDivision++)
// 	{
// 		// north
// 		indices.push_back(iNorthPole);
// 		indices.push_back(ResolveIndex(0, currHorizontalDivision));
// 		indices.push_back(ResolveIndex(0, currHorizontalDivision + 1));
// 		// south
// 		indices.push_back(ResolveIndex(verticalDivisions - 2, currHorizontalDivision + 1));
// 		indices.push_back(ResolveIndex(verticalDivisions - 2, currHorizontalDivision));
// 		indices.push_back(iSouthPole);
// 	}
// 	// wrap triangles
// 	// north
// 	indices.push_back(iNorthPole);
// 	indices.push_back(ResolveIndex(0, horizontalDivisions - 1));
// 	indices.push_back(ResolveIndex(0, 0));
// 	// south
// 	indices.push_back(ResolveIndex(verticalDivisions - 2, 0));
// 	indices.push_back(ResolveIndex(verticalDivisions - 2, horizontalDivisions - 1));
// 	indices.push_back(iSouthPole);
// 
// 	return { std::move(vertices),std::move(indices) };
// }
// 
// VOID Sphere::Update(FLOAT DeltaTime) noexcept
// {
// 	pitch += dpitch * DeltaTime;
// 	yaw += dyaw * DeltaTime;
// 	roll += droll * DeltaTime;
// 
// 	theta += dtheta * DeltaTime;
// 	phi += dphi * DeltaTime;
// 	chi += dchi * DeltaTime;
// }
// 
// DirectX::XMMATRIX Sphere::GetTranformMatrix() const noexcept
// {
// 	return
// 		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
// 		DirectX::XMMatrixTranslation(r, 0.0, 0.0) *
// 		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
// 		DirectX::XMMatrixTranslation(0.0, 0.0, 20.0);
// }