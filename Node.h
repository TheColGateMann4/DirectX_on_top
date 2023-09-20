#pragma once
#include "Includes.h"
#include "Shape.h"
#include "Mesh.h"
#include "imgui/imgui.h"
#include "Model.h"

class Node
{
	friend class Model;

public:
	struct ModelMaterial {
		BOOL normalMapEnabled = TRUE;
		BOOL normalMapHasAlpha = FALSE;
		BOOL specularMapEnabled = TRUE;
		float specularPower = 3.1f;
		DirectX::XMFLOAT3 specularColor = { 0.75f,0.75f,0.75f };
		float specularMapWeight = 0.671f;

		float padding[4];
	};
	struct ModelMaterialNoMaps {
		DirectX::XMFLOAT4 materialColor = { 0.447970f,0.327254f,0.176283f,1.0f };
		DirectX::XMFLOAT4 specularColor = { 0.65f,0.65f,0.65f,1.0f };
		float specularPower = 120.0f;

		float padding[3];
	};

public:
	Node(std::vector<Mesh*> pMeshes, const DirectX::XMMATRIX &transform, const char* nodeName) noexcept(!IS_DEBUG)
		: m_pMeshes(std::move(pMeshes)), m_nodeName(nodeName)
	{
		DirectX::XMStoreFloat4x4(&m_baseTransform, transform);
	}

public:
	void Draw(GFX &gfx, DirectX::XMMATRIX transform) const noexcept(!IS_DEBUG)
	{
		const auto finalTransform = 
			(
				DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *		//added rotation
				DirectX::XMMatrixTranslation(position.x, position.y, position.z) *				//added position
				DirectX::XMMatrixScaling(scale.x, scale.y, scale.z)								//added scale
			)
			* DirectX::XMLoadFloat4x4(&m_baseTransform)											// original transform in node
			* transform;																		// accumulated transform from upper nodes

		for (const auto& mesh : m_pMeshes)
		{
			mesh->Draw(gfx, finalTransform);
		}

		//passing accumulated transform to objects lower in hierarchy
		for (const auto& pChild : m_pChildrens)
		{
			pChild->Draw(gfx, finalTransform);
		}
	}

	void ResetLocalTranform()
	{
		position = { 0.0f, 0.0f, 0.0f };
		rotation = { 0.0f, 0.0f, 0.0f };
		scale = { 1.0f, 1.0f, 1.0f };
	}

public:
	void GenerateTree(Node*& pressedNode)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

		if (m_pChildrens.empty())
			flags |= ImGuiTreeNodeFlags_Leaf;
		if(pressedNode == this)
			flags |= ImGuiTreeNodeFlags_Selected;

		const bool nodeExpanded = ImGui::TreeNodeEx(m_nodeName.c_str(), flags);

		if (ImGui::IsItemClicked())
			if (pressedNode != this)
				pressedNode = this;
			else
				pressedNode = nullptr;

		if (nodeExpanded)
		{
			for (const auto& child : m_pChildrens)
				child->GenerateTree(pressedNode);

			ImGui::TreePop();
		}
	}
	/*
	
		struct ModelMaterial {
		BOOL normalMapEnabled = TRUE;
		BOOL normalMapHasAlpha = FALSE;
		BOOL specularMapEnabled = TRUE;
		float specularPower = 3.1f;
		DirectX::XMFLOAT3 specularColor = { 0.75f,0.75f,0.75f };
		float specularMapWeight = 0.671f;

		float padding[4];
	};
	struct ModelMaterialNoMaps {
		DirectX::XMFLOAT4 materialColor = { 0.447970f,0.327254f,0.176283f,1.0f };
		DirectX::XMFLOAT4 specularColor = { 0.65f,0.65f,0.65f,1.0f };
		float specularPower = 120.0f;

		float padding[3];
	};

	*/
	void GenerateShaderOptions(GFX& gfx)
	{
		if (m_pMeshes.empty())
			return;

		if (auto bindableShaderMaterial = m_pMeshes.front()->GetBindable<PixelConstantBuffer<ModelMaterial>>())
		{
			bool normalMapEnabledChanged = ImGui::Checkbox("normalMapEnabled", (bool*)(&shaderMaterial.normalMapEnabled));
			bool normalMapHasAlphaChanged = ImGui::Checkbox("normalMapHasAlpha", (bool*)(&shaderMaterial.normalMapHasAlpha));
			bool specularMapEnabledChanged = ImGui::Checkbox("specularMapEnabled", (bool*)(&shaderMaterial.specularMapEnabled));
			bool specularPowerChanged = ImGui::SliderFloat("specularPower", &shaderMaterial.specularPower, 0.0f, 1000.0f, "%f");
			bool specularColorChanged = ImGui::ColorPicker3("specularColor", (float*)(&shaderMaterial.specularColor.x));
			bool specularMapWeightChanged = ImGui::SliderFloat("specularMapWeight", &shaderMaterial.specularMapWeight, 0.0f, 2.0f);

			if (normalMapEnabledChanged || normalMapHasAlphaChanged || specularMapEnabledChanged || specularPowerChanged || specularColorChanged || specularMapWeightChanged)
				bindableShaderMaterial->Update(gfx, shaderMaterial);
		}
		else if (auto bindableShaderMaterial = m_pMeshes.front()->GetBindable<PixelConstantBuffer<ModelMaterialNoMaps>>())
		{
			bool materialColorChanged = ImGui::ColorPicker4("materialColor", (float*)(&shaderMaterialNoMaps.materialColor.x));
			bool specularColorChanged = ImGui::ColorPicker4("specularColor", (float*)(&shaderMaterialNoMaps.specularColor.x));
			bool specularPowerChanged = ImGui::SliderFloat("specularPower", &shaderMaterialNoMaps.specularPower, 0.0f, 1000.0f, "%f");

			if(materialColorChanged || specularColorChanged || specularPowerChanged)
				bindableShaderMaterial->Update(gfx, shaderMaterialNoMaps);
		}
	}

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
	{
		assert(pChild);
		m_pChildrens.push_back(std::move(pChild));
	}

private:
	std::vector<std::unique_ptr<Node>> m_pChildrens;
	std::vector<Mesh*> m_pMeshes;
	DirectX::XMFLOAT4X4 m_baseTransform;
	std::string m_nodeName;
	ModelMaterial shaderMaterial;
	ModelMaterialNoMaps shaderMaterialNoMaps;

public:
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
};

