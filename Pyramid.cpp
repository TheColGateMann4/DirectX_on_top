// #include "Pyramid.h"
// #include "BindableClassesMacro.h"
// 
// Pyramid::Pyramid(GFX& gfx, std::mt19937& rng,
// 	std::uniform_real_distribution<float>& adist,
// 	std::uniform_real_distribution<float>& ddist,
// 	std::uniform_real_distribution<float>& odist,
// 	std::uniform_real_distribution<float>& rdist)
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
// 		struct Vertex
// 		{
// 			struct
// 			{
// 				FLOAT x, y, z;
// 			} pos;
// 		};
// 		Mesh<Vertex> PyramidModel = GetMesh<Vertex>();
// 
// 		AddStaticBind(std::make_unique<VertexBuffer>(gfx, PyramidModel.m_vertices));
// 
// 		std::unique_ptr<VertexShader> pVertexShader = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
// 		ID3DBlob* pBlob = pVertexShader->GetByteCode();
// 		AddStaticBind(std::move(pVertexShader));
// 
// 
// 		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));
// 
// 		AddStaticIndexBufferBind(std::make_unique<IndexBuffer>(gfx, PyramidModel.m_indices));
// 
// 
// 		struct ConstantBufferColor
// 		{
// 			struct
// 			{
// 				float r, g, b, a;
// 			}face_colors[6];
// 		};
// 
// 		const ConstantBufferColor constbufferColor
// 		{
// 			{
// 				{ 1.0,0.0,1.0 },
// 				{ 1.0,0.0,0.0 },
// 				{ 0.0,1.0,0.0 },
// 				{ 0.0,0.0,1.0 },
// 				{ 1.0,1.0,0.0 },
// 				{ 0.0,1.0,1.0 },
// 			}
// 		};
// 		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBufferColor>>(gfx, constbufferColor));
// 
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
// 	else
// 	{
// 		GetIndexBufferFromVector();
// 	}
// 
// 	AddBindable(std::make_unique<TransformConstBuffer>(gfx, *this));
// }
// 
// template <class T>
// static Mesh<T> Pyramid::GetMesh()
// {
// 	std::vector<T> vertices =
// 	{
// 		{ -1.0f,-1.0f,-1.0f },
// 		{ 1.0f, -1.0f, -1.0f },
// 		{ -1.0f,-1.0f,1.0f },
// 		{ 1.0f,-1.0f,1.0f },
// 		{ 0.0f,1.0f,0.0f },
// 		{ 0.0f,-1.0f,-1.0f },
// 		{ 1.0f, -1.0f, 0.0f },
// 		{ -1.0f,-1.0f,0.0f },
// 		{ 0.0f,-1.0f,1.0f },
// 	};
// 
// 	std::vector<UINT32> indices =
// 	{
// 		0,1,2,	1,3,2,
// 		4,8,3,	4,2,8,
// 		4,6,1,	4,3,6,
// 		4,7,2,	4,0,7,
// 		4,5,0,  4,1,5,
// 	};
// 
// 	return { std::move(vertices), std::move(indices) };
// }
// 
// 
// VOID Pyramid::Update(FLOAT DeltaTime) noexcept
// {
// 	roll += droll * DeltaTime;
// 	pitch += dpitch * DeltaTime;
// 	yaw += dyaw * DeltaTime;
// 
// 	theta += dtheta * DeltaTime;
// 	phi += dphi * DeltaTime;
// 	chi += dchi * DeltaTime;
// }
// 
// DirectX::XMMATRIX Pyramid::GetTranformMatrix() const noexcept
// {
// 	return
// 		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
// 		DirectX::XMMatrixTranslation(r, 0.0, 0.0) *
// 		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
// 		DirectX::XMMatrixTranslation(0.0, 0.0, 20.0);
// }