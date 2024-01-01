#include "Cube.h"
#include "BindableClassesMacro.h"
#include "Vertex.h"
#include "imgui/imgui.h"

#include "RenderTechnique.h"
#include "RenderPass.h"
#include "RenderSteps.h"

Cube::Cube(GFX& gfx, float scale, std::string diffuseTexture, std::string normalTexture)
{
	SimpleMesh CubeModel = GetUnwrappedMesh(scale, true);


	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, GetName(), CubeModel.m_indices);
	m_pVertexBuffer = VertexBuffer::GetBindable(gfx, GetName(), CubeModel.m_vertices);
	m_pTopology = Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pTransformConstBuffer = std::make_shared<TransformConstBufferWithPixelShader>(gfx, *this, 0, 2);

	{
		RenderTechnique normalTechnique;

		{
			RenderSteps normalStep(PASS_NORMAL);

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Phong_Cube.cso");
			ID3DBlob* pBlob = pVertexShader->GetByteCode();

			DynamicConstantBuffer::BufferLayout layout;
			layout.Add<DynamicConstantBuffer::DataType::Float>("specularIntensity");
			layout.Add<DynamicConstantBuffer::DataType::Float>("specularPower");
			layout.Add<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled");
			layout.Add<DynamicConstantBuffer::DataType::Float>("normalMapWeight");

			DynamicConstantBuffer::BufferData bufferData(std::move(layout));
			*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularIntensity") = 2.0f;
			*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularPower") = 150.0f;
			*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled") = (size_t)1;
			*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("normalMapWeight") = 1.0f;


			normalStep.AddBindable(std::move(pVertexShader));

			normalStep.AddBindable(PixelShader::GetBindable(gfx, "PS_Phong_Texture_Normals_Specular_Cube.cso"));

			normalStep.AddBindable(std::make_shared<CachedBuffer>(gfx, bufferData, 1, true));

			normalStep.AddBindable(Texture::GetBindable(gfx, diffuseTexture, 0, true));

			normalStep.AddBindable(Texture::GetBindable(gfx, normalTexture, 1, true));

			normalStep.AddBindable(InputLayout::GetBindable(gfx, CubeModel.GetLayout(), pBlob));

			normalStep.AddBindable(SamplerState::GetBindable(gfx, D3D11_TEXTURE_ADDRESS_WRAP));

			normalTechnique.AddRenderStep(normalStep);
		}

		AddRenderTechnique(normalTechnique);
	}

	{
		RenderTechnique outlineTechnique;

		{
			RenderSteps maskStep(PASS_WRITE);

			outlineTechnique.AddRenderStep(maskStep);
		}

		{
			RenderSteps maskStep(PASS_MASK);


			std::shared_ptr pVertexShader = VertexShader::GetBindable(gfx, "VS_Outline.cso");
			ID3DBlob* pBlob = pVertexShader->GetByteCode();


			DynamicConstantBuffer::BufferLayout vertexBufferLayout;
			vertexBufferLayout.Add<DynamicConstantBuffer::DataType::Float>("scaleFactor");

			DynamicConstantBuffer::BufferData vertexBufferData(std::move(vertexBufferLayout));
			*vertexBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("scaleFactor") = 1.05f;


			maskStep.AddBindable(CachedBuffer::GetBindable(gfx, vertexBufferData, 1, false));


			DynamicConstantBuffer::BufferLayout PixelbufferLayout;
			PixelbufferLayout.Add<DynamicConstantBuffer::DataType::Float4>("color");

			DynamicConstantBuffer::BufferData pixelBufferData(std::move(PixelbufferLayout));
			*pixelBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("color") = { 0.0f, 1.0f, 1.0f, 1.0f };


			maskStep.AddBindable(pVertexShader);

			maskStep.AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));

			maskStep.AddBindable(std::make_shared<CachedBuffer>(gfx, pixelBufferData, 1, true));

			maskStep.AddBindable(InputLayout::GetBindable(gfx, { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } }, pBlob));

			outlineTechnique.AddRenderStep(maskStep);
		}

		AddRenderTechnique(outlineTechnique);
	}
}

void Cube::ResetLocalTransform() noexcept
{
	m_position = {};
	m_rotation = {};
	m_scale = { 1.0f, 1.0f, 1.0f };
}

void Cube::MakePropeties(GFX & gfx, float deltaTime)
{
	if (!GetPressedState())
		return;

	ImGui::Text("Position");
	ImGui::SliderFloat("pX", &m_position.x, -30.0f, 30.0f);
	ImGui::SliderFloat("pY", &m_position.y, -30.0f, 30.0f);
	ImGui::SliderFloat("pZ", &m_position.z, -30.0f, 30.0f);

	ImGui::Text("Rotation");
	ImGui::SliderFloat("rX", &m_rotation.x, -_Pi, _Pi);
	ImGui::SliderFloat("rY", &m_rotation.y, -_Pi, _Pi);
	ImGui::SliderFloat("rZ", &m_rotation.z, -_Pi, _Pi);

	ImGui::Text("Scale");
	ImGui::SliderFloat("sX", &m_scale.x, 0.01f, 100.0f);
	ImGui::SliderFloat("sY", &m_scale.y, 0.01f, 100.0f);
	ImGui::SliderFloat("sZ", &m_scale.z, 0.01f, 100.0f);

	if (ImGui::Button("Reset"))
		ResetLocalTransform();

	CachedBuffer* cachedBuffer = GetBindable<CachedBuffer>(0,0,1);

	if (!m_materialsDefined)
		shaderMaterial = cachedBuffer->constBufferData;

	ImGui::Text("Mesh Settings");

	if (ImGui::Checkbox("EnableMesh", &m_objectMeshEnabled))
	{
		this->SetTechniqueActive(0, 0, m_objectMeshEnabled);
	}

	bool changed = false;

	auto checkChanged = [&changed](bool returnFromStatement) mutable
		{
			changed = changed || returnFromStatement;
		};

	BOOL* normalMapEnabled = shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled");

	checkChanged(ImGui::SliderFloat("specularPower", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularPower"), 0.001f, 150.0f));
	checkChanged(ImGui::SliderFloat("specularIntensity", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularIntensity"), 0.001f, 150.0f));
	checkChanged(ImGui::Checkbox("normalMapEnabled", (bool*)normalMapEnabled));
	checkChanged(ImGui::SliderFloat("normalMapWeight", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("normalMapWeight"), 0.1f, 5.0f, "%.2f"));


	if (changed)
		cachedBuffer->Update(gfx, shaderMaterial);

	m_materialsDefined = true;

	ImGui::Text("Glow Settings");

	if (ImGui::Checkbox("EnableGlow", &m_objectGlowEnabled))
	{
		this->SetTechniqueActive(1, 1, m_objectGlowEnabled);
	}

	if (m_objectGlowEnabled)
	{
		{
			CachedBuffer* cachedOutlineBuffer = GetBindable<CachedBuffer>(1, 1, 1, true);

			DynamicConstantBuffer::BufferData bufferData = cachedOutlineBuffer->constBufferData;

			bool outlineColorChanged = ImGui::ColorPicker3("color", (float*)bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("color"));

			if (outlineColorChanged)
				cachedOutlineBuffer->Update(gfx, bufferData);
		}


		{
			CachedBuffer* cachedOutlineFactorBuffer = GetBindable<CachedBuffer>(1, 1, 1, false);

			DynamicConstantBuffer::BufferData bufferData = cachedOutlineFactorBuffer->constBufferData;

			bool outlineFactorChanged = ImGui::SliderFloat("ScaleFactor", (float*)bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("scaleFactor"), 1.001f, 2.0f);
		
			if (outlineFactorChanged)
				cachedOutlineFactorBuffer->Update(gfx, bufferData);
		}
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
	return DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z) *
		DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z) *
		DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
}