// #include "Cube.h"
// #include "BindableClassesMacro.h"
// #include "Vertex.h"
// 
// Cube::Cube(GFX& gfx,
// 	std::mt19937& rng,
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
// 	struct Vertex
// 	{
// 		struct
// 		{
// 			FLOAT x, y, z;
// 		} pos;
// 	};
// 
// 	SimpleMesh CubeModel = GetUnwrappedMesh(0.8f);
// 
// 	AddBindable(VertexBuffer::GetBindable(gfx, "Cube", CubeModel.m_vertices));
// 
// 	std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VertexShader.cso");
// 	ID3DBlob* pBlob = pVertexShader->GetByteCode();
// 	AddBindable(std::move(pVertexShader));
// 
// 
// 	AddBindable(PixelShader::GetBindable(gfx, "PixelShader.cso"));
// 
// 	AddIndexBuffer(IndexBuffer::GetBindable(gfx, "Cube", CubeModel.m_indices));
// 
// 
// 	DynamicConstantBuffer::BufferLayout layout;
// 	layout.Add<DynamicConstantBuffer::Array>("colors");
// 	layout["colors"]->Add<DynamicConstantBuffer::Float3>(6);
// 
// 	DynamicConstantBuffer::BufferData bufferData(std::move(layout));
// 
// 	bufferData += std::vector<DirectX::XMFLOAT3>{{ 1.0, 0.0, 1.0 },{ 1.0,0.0,0.0 },{ 0.0,1.0,0.0 },{ 0.0,0.0,1.0 },{ 1.0,1.0,0.0 },{ 0.0,1.0,1.0 }};
// 
// 
// 	AddBindable(CachedBuffer::GetBindable(gfx, bufferData, 0));
// 
// 	AddBindable(InputLayout::GetBindable(gfx, CubeModel.GetLayout(), pBlob));
// 
// 	AddBindable(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
// 
// 	AddBindable(std::make_shared<TransformConstBuffer>(gfx, *this));
// 
// 	DirectX::XMStoreFloat3x3(
// 		&ModelScale,
// 		DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)
// 	);
// }
// 
// SimpleMesh Cube::GetNormalMesh(float scale)
// {
// 	std::vector<DirectX::XMFLOAT3> vertices
// 	{
// 		{ -scale,-scale,-scale },
// 		{ scale, -scale, -scale },
// 		{ -scale,scale,-scale },
// 		{ scale,scale,-scale },
// 		{ -scale,-scale,scale },
// 		{ scale,-scale,scale },
// 		{ -scale,scale,scale },
// 		{ scale,scale,scale }
// 	};
// 
// 	DynamicVertex::VertexLayout layout = DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D);
// 	DynamicVertex::VertexBuffer vertexBuffer(std::move(layout));
// 	for (const auto& position : vertices)
// 		vertexBuffer.Emplace_Back(position);
// 
// 	std::vector<UINT32> indices =
// 	{
// 		0,2,1, 2,3,1,
// 		1,3,5, 3,7,5,
// 		2,6,3, 3,6,7,
// 		4,5,7, 4,7,6,
// 		0,4,2, 2,4,6,
// 		0,1,4, 1,5,4
// 	};
// 
// 	return { std::move(vertexBuffer), std::move(indices) };
// }
// 
// SimpleMesh Cube::GetUnwrappedMesh(float scale)
// {
// 	std::vector<DirectX::XMFLOAT3> vertices =
// 	{
// 		{ -scale, -scale,-scale },
// 		{ -scale, scale, -scale },
// 		{ scale, -scale, -scale },
// 		{ scale,scale,-scale },
// 		{ -scale,scale,scale },
// 		{ -scale,-scale,scale },
// 		{ scale, scale, scale},
// 		{ scale, -scale, scale},
// 		{-scale, scale, scale},
// 		{ scale, scale, scale},
// 		{-scale, -scale, scale},
// 		{scale, -scale, scale},
// 		{-scale, scale, scale},
// 		{-scale, -scale, scale}
// 	};
// 	DynamicVertex::VertexLayout layout = DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D);
// 	DynamicVertex::VertexBuffer vertexBuffer(std::move(layout));
// 	for (const auto& position : vertices)
// 		vertexBuffer.Emplace_Back(position);
// 
// 	std::vector<UINT32> indices =
// 	{
// 		1,2,0, 3,2,1,	 // Front
// 		4,0,5, 1,0,4,	 // Right
// 		3,6,7, 3,7,2,	 // Left
// 		8,3,1, 9,3,8,	 // Top
// 		0,2,10, 2,11,10, // Down
// 		6,12,13, 6,13,7	 // Back
// 	};
// 
// 	return { std::move(vertexBuffer), std::move(indices) };
// }
// 
// DirectX::XMMATRIX Cube::GetTranformMatrix() const noexcept
// {
// 	return
// 		DirectX::XMLoadFloat3x3(&ModelScale) *
// 		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
// 		DirectX::XMMatrixTranslation(r, 0.0, 0.0) *
// 		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
// }