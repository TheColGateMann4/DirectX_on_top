#include "Cube.h"
#include "BindableClassesMacro.h"

Cube::Cube(GFX& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist)
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
	struct Vertex
	{
		struct
		{
			FLOAT x, y, z;
		} pos;
	};

	SimpleMesh<Vertex> CubeModel = GetUnwrappedMesh<Vertex>(0.8f);

	AddBindable(std::make_unique<VertexBuffer>(gfx, CubeModel.m_vertices));

	std::unique_ptr<VertexShader> pVertexShader = std::make_unique<VertexShader>(gfx, "VertexShader.cso");
	ID3DBlob* pBlob = pVertexShader->GetByteCode();
	AddBindable(std::move(pVertexShader));


	AddBindable(std::make_unique<PixelShader>(gfx, "PixelShader.cso"));

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, CubeModel.m_indices));


	struct ConstantBufferColor
	{
		struct
		{
			float r, g, b, a;
		}face_colors[6];
	};

	const ConstantBufferColor constbufferColor
	{
		{
			{ 1.0,0.0,1.0 },
			{ 1.0,0.0,0.0 },
			{ 0.0,1.0,0.0 },
			{ 0.0,0.0,1.0 },
			{ 1.0,1.0,0.0 },
			{ 0.0,1.0,1.0 },
		}
	};
	AddBindable(std::make_unique<PixelConstantBuffer<ConstantBufferColor>>(gfx, constbufferColor, 0));


	const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	AddBindable(std::make_unique<InputLayout>(gfx, inputElementDesc, pBlob));

	AddBindable(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBindable(std::make_unique<TransformConstBuffer>(gfx, *this));

	DirectX::XMStoreFloat3x3(
		&ModelScale,
		DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)
	);
}

template <class T>
static SimpleMesh<T> Cube::GetNormalMesh(float scale)
{
	std::vector<T> vertices
	{
		{ -scale,-scale,-scale },
		{ scale, -scale, -scale },
		{ -scale,scale,-scale },
		{ scale,scale,-scale },
		{ -scale,-scale,scale },
		{ scale,-scale,scale },
		{ -scale,scale,scale },
		{ scale,scale,scale }
	};
	std::vector<UINT32> indices =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	return { std::move(vertices), std::move(indices) };
}

template <class T>
static SimpleMesh<T> Cube::GetUnwrappedMesh(float scale)
{
	std::vector<T> vertices =
	{
		{ -scale, -scale,-scale },
		{ -scale, scale, -scale },
		{ scale, -scale, -scale },
		{ scale,scale,-scale },
		{ -scale,scale,scale },
		{ -scale,-scale,scale },
		{ scale, scale, scale},
		{ scale, -scale, scale},
		{-scale, scale, scale},
		{ scale, scale, scale},
		{-scale, -scale, scale},
		{scale, -scale, scale},
		{-scale, scale, scale},
		{-scale, -scale, scale}
	};
	std::vector<UINT32> indices =
	{
		1,2,0, 3,2,1,	 // Front
		4,0,5, 1,0,4,	 // Right
		3,6,7, 3,7,2,	 // Left
		8,3,1, 9,3,8,	 // Top
		0,2,10, 2,11,10, // Down
		6,12,13, 6,13,7	 // Back
	};

	return { std::move(vertices), std::move(indices) };
}

DirectX::XMMATRIX Cube::GetTranformMatrix() const noexcept
{
	return
		DirectX::XMLoadFloat3x3(&ModelScale) *
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0, 0.0) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}