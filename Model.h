#pragma once
#include "Includes.h"
#include "Node.h"
#include "Bindable.h"
#include "BindableClassesMacro.h"
#include "Vertex.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "imgui/imgui.h"
#include <random>

class Model
{
public:
	Model(GFX& gfx, std::string fileName)
	{
		Assimp::Importer importer;
		const auto pScene = importer.ReadFile(fileName.c_str(),
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals
		);
		
		const char* errormesadawf = importer.GetErrorString();

		if (pScene == nullptr)
		{
			
		}

		for (size_t i = 0; i < pScene->mNumMeshes; i++)
		{
			m_pMeshes.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
		}

		m_pStartingNode = ParseNode(*pScene->mRootNode);
		m_pressedNode = m_pStartingNode.get();
	}

public:
	static std::unique_ptr<Mesh> ParseMesh(GFX& gfx, const aiMesh& mesh)
	{
		DynamicVertex::VertexBuffer vertexBuffer(std::move(
			DynamicVertex::VertexLayout()
			.Append(DynamicVertex::VertexLayout::Position3D)
			.Append(DynamicVertex::VertexLayout::Normal)
		));

		for (size_t i = 0; i < mesh.mNumVertices; i++)
		{
			vertexBuffer.Emplace_Back(
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
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


		std::vector<std::unique_ptr<Bindable>> bindables;

		bindables.push_back(std::make_unique<VertexBuffer>(gfx, vertexBuffer));

		std::unique_ptr<VertexShader> pVertexShader = std::make_unique<VertexShader>(gfx, L"VertexPhongLightningShader.cso");
		ID3DBlob* pBlob = pVertexShader->GetByteCode();
		bindables.push_back(std::move(pVertexShader));


		bindables.push_back(std::make_unique<PixelShader>(gfx, L"PixelPhongLightningShader.cso"));

		bindables.push_back(std::make_unique<IndexBuffer>(gfx, indices));

		bindables.push_back(std::make_unique<InputLayout>(gfx, vertexBuffer.GetLayout().GetDirectXLayout(), pBlob));

		struct ModelMaterial {
			alignas(16) DirectX::XMFLOAT3 color = {0.0f, 1.0f, 1.0f};
			float specularIntensity = 1.0f;
			float specularPower = 70.0f;
			float padding[2] = {};
		}material;

		bindables.push_back(std::make_unique<PixelConstantBuffer<ModelMaterial>>(gfx, material, 1));

		return std::make_unique<Mesh>(gfx, std::move(bindables));
	}

	std::unique_ptr<Node> ParseNode(const aiNode& node) noexcept
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

	void Draw(GFX &gfx) const noexcept(!IS_DEBUG)
	{
		m_pStartingNode->Draw(gfx, DirectX::XMMatrixIdentity());
	}

public:
	void ControlWindow()
	{
		if (ImGui::Begin("Object Controler"))
		{
			ImGui::Columns(2, nullptr, true);
			m_pressedNode = m_pStartingNode->GenerateTree(m_pressedNode);

			ImGui::NextColumn();

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
			}
		}
		ImGui::End();
	}

private:
	std::unique_ptr<Node> m_pStartingNode;
	std::vector<std::unique_ptr<Mesh>> m_pMeshes;

private:
	Node* m_pressedNode = nullptr;
};

