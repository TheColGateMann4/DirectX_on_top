#include "Cube.h"
#include "BindableClassesMacro.h"
#include "Vertex.h"
#include "imgui/imgui.h"

Cube::Cube(GFX& gfx, float scale, std::string diffuseTexture, std::string normalTexture)
{
	SimpleMesh CubeModel = GetUnwrappedMesh(scale, true);

	AddBindable(VertexBuffer::GetBindable(gfx, "Cube", CubeModel.m_vertices));

	std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Phong_Cube.cso");
	ID3DBlob* pBlob = pVertexShader->GetByteCode();
	AddBindable(std::move(pVertexShader));

	AddBindable(PixelShader::GetBindable(gfx, "PS_Phong_Texture_Normals_Specular_Cube.cso"));

	DynamicConstantBuffer::BufferLayout layout;
	layout.Add<DynamicConstantBuffer::DataType::Float>("specularIntensity");
	layout.Add<DynamicConstantBuffer::DataType::Float>("specularPower");
	layout.Add<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled");

	DynamicConstantBuffer::BufferData bufferData(std::move(layout));
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularIntensity") = 150.0f;
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularPower") = 2.0f;
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled") = FALSE;

	AddBindable(std::make_shared<CachedBuffer>(gfx, bufferData, 1, true));


	AddIndexBuffer(IndexBuffer::GetBindable(gfx, "Cube", CubeModel.m_indices));


// 	DynamicConstantBuffer::BufferLayout layout;
// 	layout.Add<DynamicConstantBuffer::Array>("colors");
// 	layout["colors"]->Add<DynamicConstantBuffer::Float3>(6);
// 
// 	DynamicConstantBuffer::BufferData bufferData(std::move(layout));
// 	bufferData += std::vector<DirectX::XMFLOAT3>{{ 1.0, 0.0, 1.0 },{ 1.0,0.0,0.0 },{ 0.0,1.0,0.0 },{ 0.0,0.0,1.0 },{ 1.0,1.0,0.0 },{ 0.0,1.0,1.0 }};
// 	AddBindable(CachedBuffer::GetBindable(gfx, bufferData, 0, true));

	AddBindable(Texture::GetBindable(gfx, diffuseTexture, 0));

	AddBindable(Texture::GetBindable(gfx, normalTexture, 1));

	AddBindable(InputLayout::GetBindable(gfx, CubeModel.GetLayout(), pBlob));

	AddBindable(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBindable(SamplerState::GetBindable(gfx, D3D11_TEXTURE_ADDRESS_WRAP));

	AddBindable(std::make_shared<TransformConstBufferWithPixelShader>(gfx, *this, 0, 2));
}

void Cube::ResetLocalTransform() noexcept
{
	m_position = {};
	m_rotation = {};
	m_scale = { 1.0f, 1.0f, 1.0f };
}

void Cube::SpawnControlWindow(GFX& gfx) noexcept
{
	if (ImGui::Begin("Cube"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("pX", &m_position.x, -30.0f, 30.0f);
		ImGui::SliderFloat("pY", &m_position.y, -30.0f, 30.0f);
		ImGui::SliderFloat("pZ", &m_position.z, -30.0f, 30.0f);

		ImGui::Text("Rotation");
		ImGui::SliderFloat("rX", &m_rotation.x, -std::_Pi, std::_Pi);
		ImGui::SliderFloat("rY", &m_rotation.y, -std::_Pi, std::_Pi);
		ImGui::SliderFloat("rZ", &m_rotation.z, -std::_Pi, std::_Pi);

		ImGui::Text("Scale");
		ImGui::SliderFloat("sX", &m_scale.x, 0.01f, 100.0f);
		ImGui::SliderFloat("sY", &m_scale.y, 0.01f, 100.0f);
		ImGui::SliderFloat("sZ", &m_scale.z, 0.01f, 100.0f);

		if (ImGui::Button("Reset"))
			ResetLocalTransform();

		CachedBuffer* cachedBuffer = GetBindable<CachedBuffer>();

		if (!materialsDefined)
			shaderMaterial = cachedBuffer->constBufferData;


		bool powerChanged = false, intensityChanged = false, normalMapStateChanged = false;

		powerChanged = ImGui::SliderFloat("specularPower", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularPower"), 0.001f, 150.0f);
		intensityChanged = ImGui::SliderFloat("specularIntensity", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularIntensity"), 0.001f, 150.0f);
		normalMapStateChanged = ImGui::Checkbox("normalMapEnabled", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled"));

		if(powerChanged || intensityChanged || normalMapStateChanged)
			cachedBuffer->Update(gfx, shaderMaterial);

		materialsDefined = true;

		ImGui::End();
	}
}

SimpleMesh Cube::GetNormalMesh(float scale)
{
	std::vector<DirectX::XMFLOAT3> vertices
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

	DynamicVertex::VertexLayout layout = DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D);
	DynamicVertex::VertexBuffer vertexBuffer(std::move(layout));
	for (const auto& position : vertices)
		vertexBuffer.Emplace_Back(position);

	std::vector<UINT32> indices =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	return { std::move(vertexBuffer), std::move(indices) };
}

SimpleMesh Cube::GetUnwrappedMesh(float scale, bool getExtendedStuff)
{
	/* vertices */

	const std::vector<DirectX::XMFLOAT3> vertexPositions = {

		//front
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},

		//right
		{1.0f, -1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},

		//left
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},

		//top
		{-1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, 1.0f},

		//bottom
		{1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},

		//back
		{1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f}
	};

	const std::vector<DirectX::XMFLOAT3> vertexNormals = {
		//front
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},

		//right
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},

		//left
		{-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},

		//top
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},

		//bottom
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},

		//back
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}
	};

	const std::vector<DirectX::XMFLOAT2> vertexTexturePositions = {
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
	};

	DynamicVertex::VertexLayout layout = DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D);

	if (getExtendedStuff)
	{
		layout.Append(DynamicVertex::VertexLayout::Normal);
		layout.Append(DynamicVertex::VertexLayout::Texture2D);
	}

	DynamicVertex::VertexBuffer vertexBuffer(std::move(layout));

	for (size_t i = 0; i < vertexPositions.size(); i++)
		if(getExtendedStuff)
			vertexBuffer.Emplace_Back(vertexPositions.at(i), vertexNormals.at(i), vertexTexturePositions.at(i));
		else
			vertexBuffer.Emplace_Back(vertexPositions.at(i));

	/* indices */

	const std::vector<UINT32> cubeFaceIndices = { 1,2,0, 3,2,1 };
	std::vector<UINT32> indices = {};

	for (size_t i = 0; i < 6; i++)
		for (const auto& indice : cubeFaceIndices)
			indices.push_back(indice + i * 4);


	return { std::move(vertexBuffer), std::move(indices) };
}

DirectX::XMMATRIX Cube::GetTranformMatrix() const noexcept
{
	return
		DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z) *
		DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z) *
		DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
}