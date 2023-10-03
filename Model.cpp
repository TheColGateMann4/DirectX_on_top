#include "Model.h"
#include <Shlwapi.h>



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
	//m_pressedNode = m_pStartingNode.get();
	m_scale = scale;
}

std::unique_ptr<Mesh> Model::ParseMesh(GFX& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, std::string &modelPath, std::string &modelName, float scale)
{
	bool hasSpecularMap = false;
	bool specularHasAlpha = false;
	bool hasNormalMap = false;
	bool normalMapHasAlpha = false;
	bool hasDiffuseMap = false;
	bool diffuseMapHasAlpha = false;
	float shinyness = 35.0f;

	std::vector<std::shared_ptr<Bindable>> bindables;

	DirectX::XMFLOAT4 SpecularColor = { 0.18f, 0.18f, 0.18f, 1.0f };
	DirectX::XMFLOAT4 DiffuseColor = { 0.45f, 0.45f, 0.85f, 1.0f };

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
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(DiffuseColor));
		}

		if (material.GetTexture(aiTextureType_NORMALS, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			const auto normalTexture = Texture::GetBindable(gfx, modelPath + textureFileName.C_Str(), 2);
			normalMapHasAlpha = normalTexture->HasAlpha();
			bindables.push_back(std::move(normalTexture));
			hasNormalMap = true;
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			const auto specularTexture = Texture::GetBindable(gfx, modelPath + textureFileName.C_Str(), 1);
			specularHasAlpha = specularTexture->HasAlpha();
			bindables.push_back(std::move(specularTexture));
			hasSpecularMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(SpecularColor));
		}

		if (!specularHasAlpha)
		{
			material.Get(AI_MATKEY_SHININESS, shinyness);
		}

		if (hasSpecularMap || hasDiffuseMap || hasNormalMap)
		{
			bindables.push_back(SamplerState::GetBindable(gfx, D3D11_TEXTURE_ADDRESS_WRAP));
		}
	}

	if (hasSpecularMap && hasNormalMap && hasDiffuseMap)
	{
		DynamicVertex::VertexBuffer vertexBuffer(std::move(
			DynamicVertex::VertexLayout()
			.Append(DynamicVertex::VertexLayout::Position3D)
			.Append(DynamicVertex::VertexLayout::Normal)
			.Append(DynamicVertex::VertexLayout::Tangent)
			.Append(DynamicVertex::VertexLayout::Bitangent)
			.Append(DynamicVertex::VertexLayout::Texture2D)
		));

		for (size_t i = 0; i < mesh.mNumVertices; i++)
		{
			vertexBuffer.Emplace_Back(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
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

		std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Model_Phong_Texture_Normals.cso");
		ID3DBlob* pBlob = pVertexShader->GetByteCode();
		bindables.push_back(std::move(pVertexShader));

		bindables.push_back(PixelShader::GetBindable(gfx, "PS_Model_Phong_Texture_SpecularMap_Normals.cso"));

		Node::ModelMaterial modelMaterial = {};
		modelMaterial.specularMapHasAlpha = normalMapHasAlpha;
		modelMaterial.specularPower = shinyness;
		modelMaterial.specularColor = DirectX::XMFLOAT3(SpecularColor.x, SpecularColor.y, SpecularColor.z);

		bindables.push_back(std::make_shared<PixelConstantBuffer<Node::ModelMaterial>>(PixelConstantBuffer<Node::ModelMaterial>(gfx, modelMaterial, 1)));

		bindables.push_back(InputLayout::GetBindable(gfx, vertexBuffer.GetLayout(), pBlob));
	}
	else if (hasSpecularMap && !hasNormalMap && hasDiffuseMap)
	{
		DynamicVertex::VertexBuffer vertexBuffer(std::move(
			DynamicVertex::VertexLayout()
			.Append(DynamicVertex::VertexLayout::Position3D)
			.Append(DynamicVertex::VertexLayout::Normal)
			.Append(DynamicVertex::VertexLayout::Tangent)
			.Append(DynamicVertex::VertexLayout::Bitangent)
			.Append(DynamicVertex::VertexLayout::Texture2D)
		));

		for (size_t i = 0; i < mesh.mNumVertices; i++)
		{
			vertexBuffer.Emplace_Back(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
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

		std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Model_Phong_Texture_Normals.cso");
		ID3DBlob* pBlob = pVertexShader->GetByteCode();
		bindables.push_back(std::move(pVertexShader));

		bindables.push_back(PixelShader::GetBindable(gfx, "PS_Model_Phong_Texture_SpecularMap_Normals.cso"));

		Node::ModelMaterial modelMaterial = {};
		modelMaterial.normalMapEnabled = FALSE;
		modelMaterial.specularMapHasAlpha = FALSE;
		modelMaterial.specularPower = shinyness;
		modelMaterial.specularMapWeight = 1.0f;

		bindables.push_back(std::make_shared<PixelConstantBuffer<Node::ModelMaterial>>(PixelConstantBuffer<Node::ModelMaterial>(gfx, modelMaterial, 1)));

		bindables.push_back(InputLayout::GetBindable(gfx, vertexBuffer.GetLayout(), pBlob));
	}
	else if (hasDiffuseMap && hasNormalMap)
	{
		DynamicVertex::VertexBuffer vertexBuffer(std::move(
			DynamicVertex::VertexLayout()
			.Append(DynamicVertex::VertexLayout::Position3D)
			.Append(DynamicVertex::VertexLayout::Normal)
			.Append(DynamicVertex::VertexLayout::Tangent)
			.Append(DynamicVertex::VertexLayout::Bitangent)
			.Append(DynamicVertex::VertexLayout::Texture2D)
		));

		for (size_t i = 0; i < mesh.mNumVertices; i++)
		{
			vertexBuffer.Emplace_Back(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
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

		std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Model_Phong_Texture_Normals.cso");
		ID3DBlob* pBlob = pVertexShader->GetByteCode();
		bindables.push_back(std::move(pVertexShader));

		bindables.push_back(PixelShader::GetBindable(gfx, "PS_Model_Phong_Texture_Specular_Normals.cso"));

		struct ModelMaterialDiffuseNormal {
			float specularIntensity;
			float specularPower;
			BOOL normalMapEnabled = TRUE;
			float padding;
		}modelMaterial;

		modelMaterial.specularIntensity = (SpecularColor.x + SpecularColor.y + SpecularColor.z) / 3.0f;
		modelMaterial.specularPower = shinyness;

		bindables.push_back(std::make_shared<PixelConstantBuffer<ModelMaterialDiffuseNormal>>(PixelConstantBuffer<ModelMaterialDiffuseNormal>(gfx, modelMaterial, 1)));

		bindables.push_back(InputLayout::GetBindable(gfx, vertexBuffer.GetLayout(), pBlob));
	}
	else if (hasDiffuseMap)
	{
		DynamicVertex::VertexBuffer vertexBuffer(std::move(
			DynamicVertex::VertexLayout()
			.Append(DynamicVertex::VertexLayout::Position3D)
			.Append(DynamicVertex::VertexLayout::Normal)
			.Append(DynamicVertex::VertexLayout::Texture2D)
		));

		for (size_t i = 0; i < mesh.mNumVertices; i++)
		{
			vertexBuffer.Emplace_Back(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
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

		std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Phong_Texture_Normals.cso");
		ID3DBlob* pBlob = pVertexShader->GetByteCode();
		bindables.push_back(std::move(pVertexShader));

		bindables.push_back(PixelShader::GetBindable(gfx, "PS_Phong_Texture.cso"));

		struct ModelMaterialDiffuse {
			float specularIntensity;
			float specularPower;
			float padding[2];
		}modelMaterial;

		modelMaterial.specularIntensity = (SpecularColor.x + SpecularColor.y + SpecularColor.z) / 3;
		modelMaterial.specularPower = shinyness;

		bindables.push_back(std::make_shared<PixelConstantBuffer<ModelMaterialDiffuse>>(PixelConstantBuffer<ModelMaterialDiffuse>(gfx, modelMaterial, 1)));

		bindables.push_back(InputLayout::GetBindable(gfx, vertexBuffer.GetLayout(), pBlob));
	}
	else if (!hasSpecularMap && !hasNormalMap && !hasDiffuseMap)
	{
		DynamicVertex::VertexBuffer vertexBuffer(std::move(
			DynamicVertex::VertexLayout()
			.Append(DynamicVertex::VertexLayout::Position3D)
			.Append(DynamicVertex::VertexLayout::Normal)
		));

		for (size_t i = 0; i < mesh.mNumVertices; i++)
		{
			vertexBuffer.Emplace_Back(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
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

		std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Phong_Position_Normals.cso");
		ID3DBlob* pBlob = pVertexShader->GetByteCode();
		bindables.push_back(std::move(pVertexShader));

		bindables.push_back(PixelShader::GetBindable(gfx, "PS_Phong_NoMaps.cso"));

		Node::ModelMaterialNoMaps modelMaterial = {};
		modelMaterial.materialColor = DiffuseColor;
		modelMaterial.specularColor = SpecularColor;
		modelMaterial.specularPower = shinyness;

		bindables.push_back(std::make_shared<PixelConstantBuffer<Node::ModelMaterialNoMaps>>(PixelConstantBuffer<Node::ModelMaterialNoMaps>(gfx, modelMaterial, 1)));

		bindables.push_back(InputLayout::GetBindable(gfx, vertexBuffer.GetLayout(), pBlob));
	}
	else
	{
		std::runtime_error("Wrong combination of model texture maps.");
	}

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

void Model::MakeHierarchy(GFX& gfx)
{
	ImGui::Columns(2, nullptr, true);
	m_pStartingNode->GenerateTree(m_pressedNode);
}

void Model::MakePropeties(GFX& gfx)
{
	if (m_pressedNode != nullptr)
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("pX", &m_pressedNode->position.x, -30.0f, 30.0f);
		ImGui::SliderFloat("pY", &m_pressedNode->position.y, -30.0f, 30.0f);
		ImGui::SliderFloat("pZ", &m_pressedNode->position.z, -30.0f, 30.0f);

		ImGui::Text("Rotation");
		ImGui::SliderFloat("rX", &m_pressedNode->rotation.x, -std::_Pi, std::_Pi);
		ImGui::SliderFloat("rY", &m_pressedNode->rotation.y, -std::_Pi, std::_Pi);
		ImGui::SliderFloat("rZ", &m_pressedNode->rotation.z, -std::_Pi, std::_Pi);

		ImGui::Text("Scale");
		ImGui::SliderFloat("sX", &m_pressedNode->scale.x, 0.1f, 30.0f);
		ImGui::SliderFloat("sY", &m_pressedNode->scale.y, 0.1f, 30.0f);
		ImGui::SliderFloat("sZ", &m_pressedNode->scale.z, 0.1f, 30.0f);

		if (ImGui::Button("Reset"))
			m_pressedNode->ResetLocalTranform();

		m_pressedNode->GenerateShaderOptions(gfx);
	}
}