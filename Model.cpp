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
				THROW_INTERNAL_ERROR("FAILED TO PROCESS MODEL PATH", "Model path couldn't be processed for assimp");

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

	std::vector<std::shared_ptr<Bindable>> bindables;

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

			bindables.push_back(std::move(diffuseTexture));
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
			const auto normalTexture = Texture::GetBindable(gfx, modelPath + textureFileName.C_Str(), 2);
			normalMapHasAlpha = normalTexture->HasAlpha();
			bindables.push_back(std::move(normalTexture));
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

		constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Bool>("specularMap");
		constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Bool>("specularMapHasAlpha");
		constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Float>("specularMapWeight");

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			const auto specularTexture = Texture::GetBindable(gfx, modelPath + textureFileName.C_Str(), 1);
			specularHasAlpha = specularTexture->HasAlpha();
			bindables.push_back(std::move(specularTexture));
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


		material.Get(AI_MATKEY_SHININESS, shinyness);
		constBufferData.AddLayoutElement<DynamicConstantBuffer::DataType::Float>("specularPower");
		constBufferData += shinyness;

		if (hasSpecularMap || hasDiffuseMap || hasNormalMap)
		{
			bindables.push_back(SamplerState::GetBindable(gfx, D3D11_TEXTURE_ADDRESS_WRAP));
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

	vertexBuffer.Emplace_Back_Empty(mesh.mNumVertices);

	for (size_t i = 0; i < mesh.mNumVertices; i++)
	{
		size_t currentElementIndex = 0;

		//	Position3D
		vertexBuffer[i].SetAttr<DynamicVertex::VertexLayout::VertexComponent::Position3D>(
			vertexBuffer.GetData() + vertexBuffer.GetLayout().GetByteSize() * i + vertexBuffer.GetLayout().ResolveByIndex(currentElementIndex).GetOffset(),
			DirectX::XMFLOAT3(mesh.mVertices[i].x* scale, mesh.mVertices[i].y* scale, mesh.mVertices[i].z* scale)
		);
		currentElementIndex++;

		// Normal
		if(hasNormalMap)
		{
			vertexBuffer[i].SetAttr<DynamicVertex::VertexLayout::VertexComponent::Normal>(
				vertexBuffer.GetData() + vertexBuffer.GetLayout().GetByteSize() * i + vertexBuffer.GetLayout().ResolveByIndex(currentElementIndex).GetOffset(),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
			);
			currentElementIndex++;
		}

		// Tangent BiTangent
		if (hasNormalMap || hasSpecularMap)
		{
			vertexBuffer[i].SetAttr<DynamicVertex::VertexLayout::VertexComponent::Tangent>(
				vertexBuffer.GetData() + vertexBuffer.GetLayout().GetByteSize() * i + vertexBuffer.GetLayout().ResolveByIndex(currentElementIndex).GetOffset(),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i])
			);
			currentElementIndex++;

			vertexBuffer[i].SetAttr<DynamicVertex::VertexLayout::VertexComponent::Bitangent>(
				vertexBuffer.GetData() + vertexBuffer.GetLayout().GetByteSize() * i + vertexBuffer.GetLayout().ResolveByIndex(currentElementIndex).GetOffset(),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i])
			);
			currentElementIndex++;
		}

		// Texture2D
		if (hasNormalMap || hasSpecularMap || hasDiffuseMap)
			vertexBuffer[i].SetAttr<DynamicVertex::VertexLayout::VertexComponent::Texture2D>(
				vertexBuffer.GetData() + vertexBuffer.GetLayout().GetByteSize() * i + vertexBuffer.GetLayout().ResolveByIndex(currentElementIndex).GetOffset(),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
		);
	}

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


	bindables.push_back(VertexBuffer::GetBindable(gfx, bufferUID, vertexBuffer));

	bindables.push_back(IndexBuffer::GetBindable(gfx, bufferUID, indices));



	std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, vertexShaderName);
	ID3DBlob* pBlob = pVertexShader->GetByteCode();
	bindables.push_back(std::move(pVertexShader));

	bindables.push_back(PixelShader::GetBindable(gfx, pixelShaderName));

	bindables.push_back(std::make_shared<CachedBuffer>(gfx, constBufferData, 1, true));

	bindables.push_back(InputLayout::GetBindable(gfx, vertexBuffer.GetLayout(), pBlob));

	bindables.push_back(DepthStencil::GetBindable(gfx, DepthStencil::Off));

	bindables.push_back(RasterizerState::GetBindable(gfx, diffuseMapHasAlpha));

	return std::make_unique<Mesh>(gfx, std::move(bindables));
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

void Model::Draw(GFX& gfx) const noexcept(!IS_DEBUG)
{
	m_pStartingNode->Draw(gfx, DirectX::XMMatrixIdentity());
}

void Model::MakeHierarchy(GFX&)
{
	ImGui::Columns(2, nullptr, true);

	bool previous = m_pressedNode != nullptr;

	m_pStartingNode->GenerateTree(m_pressedNode);

	if(previous != (m_pressedNode != nullptr))
		SetPressedState(m_pressedNode != nullptr);
}

void Model::MakePropeties(GFX& gfx, float deltaTime)
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

	ImGui::Text("Scale");
	ImGui::SliderFloat("sX", &m_pressedNode->scale.x, 0.01f, 100.0f);
	ImGui::SliderFloat("sY", &m_pressedNode->scale.y, 0.01f, 100.0f);
	ImGui::SliderFloat("sZ", &m_pressedNode->scale.z, 0.01f, 100.0f);

	if (ImGui::Button("Reset"))
		m_pressedNode->ResetLocalTranform();

	m_pressedNode->GenerateShaderOptions(gfx);
}