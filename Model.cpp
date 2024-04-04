#include "Model.h"
#include "DynamicConstantBuffer.h"
#include <Shlwapi.h> // PathIsDirectory



Model::Model(GFX& gfx, std::string fileName, float scale)
{
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (pScene == nullptr)
	{
		THROW_MODEL_EXCEPTION(importer.GetErrorString());
	}

	{
		size_t foundCharacter;
		if (!PathIsDirectory(fileName.data()))
			if ((foundCharacter = fileName.find_last_of('\\')) != std::string::npos || (foundCharacter = fileName.find_last_of('\\')) != std::string::npos)
			{
				m_fileName = std::string(fileName.begin() + (foundCharacter + 1), fileName.end());
				m_filePath = std::string(fileName.begin(), fileName.end() - (fileName.length() - (foundCharacter + 1)));
			}
			else
				THROW_INTERNAL_ERROR("Model path couldn't be processed for assimp", true);

	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		m_pMeshes.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, m_filePath, m_fileName, scale));
	}

	m_pStartingNode = ParseNode(*pScene->mRootNode);
	m_scale = scale;
}

std::unique_ptr<Mesh> Model::ParseMesh(GFX& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, std::string& modelPath, std::string& modelName, float scale)
{
	bool hasSpecularMap = false;
	bool specularHasAlpha = false;
	bool hasNormalMap = false;
	bool normalMapHasAlpha = false;
	bool hasDiffuseMap = false;
	bool diffuseMapHasAlpha = false;
	float shinyness = 35.0f;

	std::string pixelShaderName = "PS_Phong", vertexShaderName = "VS_Phong";

	std::vector<std::shared_ptr<Bindable>> normalMeshBindables;

	DirectX::XMFLOAT4 SpecularColor = { 0.18f, 0.18f, 0.18f, 1.0f };
	DirectX::XMFLOAT4 DiffuseColor = { 0.45f, 0.45f, 0.85f, 1.0f };

	DynamicConstantBuffer::BufferData constBufferData;

	if (mesh.mMaterialIndex >= 0)
	{
		const aiMaterial& material = *pMaterials[mesh.mMaterialIndex];
		aiString textureFileName;

		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			auto diffuseTexture = Texture::GetBindable(gfx, modelPath + textureFileName.C_Str(), 0);
			diffuseMapHasAlpha = diffuseTexture->HasAlpha();

			normalMeshBindables.push_back(std::move(diffuseTexture));
			hasDiffuseMap = true;

			pixelShaderName += "_Texture";
			if (diffuseMapHasAlpha)
				pixelShaderName += "_Mask";
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(DiffuseColor));
			constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Float4>("materialColor");
			constBufferData += DiffuseColor;
		}

		constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Bool>("normalMap");
		constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Bool>("normalMapHasAlpha");

		if (material.GetTexture(aiTextureType_NORMALS, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			auto normalTexture = Texture::GetBindable(gfx, modelPath + textureFileName.C_Str(), 2);
			normalMapHasAlpha = normalTexture->HasAlpha();
			normalMeshBindables.push_back(std::move(normalTexture));
			hasNormalMap = true;

			constBufferData += (BOOL)hasNormalMap;
			constBufferData += (BOOL)normalMapHasAlpha;

			pixelShaderName += "_Normals";
		}
		else
		{
			constBufferData += (BOOL)false;
			constBufferData += (BOOL)false;
		}
		/*
		
			checkChanged(ImGui::SliderFloat("specularPowerChanged", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularPower"), 0.0f, 1000.0f, "%.2f"));
			checkChanged(ImGui::SliderFloat("specularMapWeight", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularMapWeight"), 0.0f, 2.0f));
		*/
		constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Bool>("specularMap");
		constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Bool>("specularMapHasAlpha");

		{
			DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo floatInfo = {};
			floatInfo.v_min = 0.0f;
			floatInfo.v_max = 2.0f;
			constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Float>("specularMapWeight", &floatInfo);
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			auto specularTexture = Texture::GetBindable(gfx, modelPath + textureFileName.C_Str(), 1);
			specularHasAlpha = specularTexture->HasAlpha();
			normalMeshBindables.push_back(std::move(specularTexture));
			hasSpecularMap = true;

			constBufferData += (BOOL)true;
			constBufferData += (BOOL)specularHasAlpha;

			pixelShaderName += "_SpecularMap";

		}
		else
		{
			constBufferData += (BOOL)false;
			constBufferData += (BOOL)false;

			if(hasDiffuseMap)
				pixelShaderName += "_Specular";
		}

		constBufferData += 1.0f;

		material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(SpecularColor));

		constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Float4>("specularColor");
		constBufferData += SpecularColor;


		//getting shinyness
		{
			material.Get(AI_MATKEY_SHININESS, shinyness);

			{
				DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo floatInfo = {};
				floatInfo.v_min = 0.1f;
				floatInfo.v_max = 150.0f;
				floatInfo.format = "%.2f";

				constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Float>("specularPower", &floatInfo);
			}

			constBufferData += shinyness;
		}

		if (hasSpecularMap || hasDiffuseMap || hasNormalMap)
		{
			vertexShaderName += "_Texture_Normals";
		}
		else
		{
			pixelShaderName += "_NoMaps";
			vertexShaderName += "_Normals";
		}
		pixelShaderName += ".cso";
		vertexShaderName += ".cso";
	}

	DynamicVertex::VertexLayout vertexBufferLayout = {};

	vertexBufferLayout.Append(DynamicVertex::VertexLayout::Position3D);
	vertexBufferLayout.Append(DynamicVertex::VertexLayout::Normal);

	if (hasNormalMap || hasSpecularMap)
	{
		vertexBufferLayout.Append(DynamicVertex::VertexLayout::Tangent);
		vertexBufferLayout.Append(DynamicVertex::VertexLayout::Bitangent);
	}

	if (hasDiffuseMap || hasNormalMap || hasSpecularMap)
		vertexBufferLayout.Append(DynamicVertex::VertexLayout::Texture2D);


	DynamicVertex::VertexBuffer vertexBuffer(std::move(vertexBufferLayout));

	vertexBuffer.SetScale(scale);
	vertexBuffer.MakeFromMesh(mesh);


	std::vector<UINT32> indices;
	indices.reserve(mesh.mNumFaces * 3);

	for (size_t i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}



	std::string bufferUID = modelPath + mesh.mName.C_Str();

	std::shared_ptr<VertexShader> pNormalVertexShader = VertexShader::GetBindable(gfx, vertexShaderName);

	std::unique_ptr<Mesh> resultMesh = std::make_unique<Mesh>();

	resultMesh->m_pIndexBuffer = IndexBuffer::GetBindable(gfx, bufferUID, indices);
	resultMesh->m_pVertexBuffer = VertexBuffer::GetBindable(gfx, bufferUID, vertexBuffer);
	resultMesh->m_pTopology = Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	resultMesh->m_pTransformConstBuffer = std::make_shared<TransformConstBufferWithPixelShader>(gfx, *resultMesh, 0, 2);

	{
		RenderTechnique normalTechnique("normal");

		{
			RenderStep normalStep("normalPass");


			normalStep.AddBindable(PixelShader::GetBindable(gfx, pixelShaderName));

			normalStep.AddBindable(std::make_shared<CachedBuffer>(gfx, constBufferData, 1, true));

			normalStep.AddBindable(RasterizerState::GetBindable(gfx, diffuseMapHasAlpha));

			normalStep.AddBindable(InputLayout::GetBindable(gfx, vertexBuffer.GetLayout(), pNormalVertexShader.get()));

			normalStep.AddBindable(std::move(pNormalVertexShader));

			//bindables added specially depending on what model uses
			for (auto& bindable : normalMeshBindables)
				normalStep.AddBindable(bindable);


			normalTechnique.AddRenderStep(normalStep);
		}

		resultMesh->AddRenderTechnique(normalTechnique);
	}

	{
		RenderTechnique outlineTechnique("outline", false);

		{
			RenderStep maskStep("outlineWriteMaskPass");

			outlineTechnique.AddRenderStep(maskStep);
		}

		{
			RenderStep maskStep("outlineMaskPass");


			std::shared_ptr<VertexShader> pMaskVertexShader = VertexShader::GetBindable(gfx, "VS.cso");

			DynamicConstantBuffer::BufferLayout PixelbufferLayout;
			PixelbufferLayout.Add<DynamicConstantBuffer::DataType::Float4>("color");

			DynamicConstantBuffer::BufferData pixelBufferData(std::move(PixelbufferLayout));
			*pixelBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("color") = { 0.0f, 1.0f, 1.0f, 1.0f };

			maskStep.AddBindable(pMaskVertexShader);

			maskStep.AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));

			maskStep.AddBindable(std::make_shared<CachedBuffer>(gfx, pixelBufferData, 1, true));

			maskStep.AddBindable(InputLayout::GetBindable(gfx, { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } }, pMaskVertexShader.get()));

			outlineTechnique.AddRenderStep(maskStep);
		}

		resultMesh->AddRenderTechnique(outlineTechnique);
	}

	return resultMesh;
}

std::unique_ptr<Node> Model::ParseNode(const aiNode& node) noexcept
{
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> pMeshes;
	pMeshes.reserve(node.mNumMeshes);

	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIndex = node.mMeshes[i];
		pMeshes.push_back(m_pMeshes.at(meshIndex).get());
	}

	auto pNode = std::make_unique<Node>(std::move(pMeshes), transform, node.mName.C_Str());
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(*node.mChildren[i]));
	}

	return pNode;
}

void Model::LinkSceneObjectToPipeline(class RenderGraph& renderGraph)
{
	for (auto& mesh : m_pMeshes)
		m_pStartingNode->LinkToPipeline(renderGraph);
}

void Model::MakeHierarchy(GFX&)
{
	ImGui::Columns(2, nullptr, true);

	Node* previous = m_pressedNode;

	m_pStartingNode->GenerateTree(m_pressedNode);

	if (previous != m_pressedNode)
	{
		if(m_pressedNode != m_pStartingNode.get())
			if(m_pressedNode != nullptr)
				SetShape(m_pressedNode->m_pMeshes.front());
			else
				SetShape(nullptr);

		SetPressedState(m_pressedNode != nullptr);
	}
}

void Model::MakeTransformPropeties(GFX& gfx)
{
	if (!GetPressedState())
		return;

	ImGui::Text("Position");
	ImGui::SliderFloat("pX", &m_pressedNode->position.x, -30.0f, 30.0f);
	ImGui::SliderFloat("pY", &m_pressedNode->position.y, -30.0f, 30.0f);
	ImGui::SliderFloat("pZ", &m_pressedNode->position.z, -30.0f, 30.0f);

	ImGui::Text("Rotation");
	ImGui::SliderFloat("rX", &m_pressedNode->rotation.x, -_Pi, _Pi);
	ImGui::SliderFloat("rY", &m_pressedNode->rotation.y, -_Pi, _Pi);
	ImGui::SliderFloat("rZ", &m_pressedNode->rotation.z, -_Pi, _Pi);

	if (ImGui::Button("Reset"))
		m_pressedNode->ResetLocalTranform();
}