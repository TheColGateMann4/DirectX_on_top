#pragma once
#include "Includes.h"
#include "Shape.h"
#include "Mesh.h"
#include "imgui/imgui.h"
#include "Model.h"
#include "DynamicConstantBuffer.h"

class Node
{
	friend class Model;

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

	void GenerateShaderOptions(GFX& gfx)
	{
		if (m_pMeshes.empty())
			return;

		auto bindableShaderMaterial = m_pMeshes.front()->GetBindable<CachedBuffer>();

		if (!materialsDefined)
			shaderMaterial = bindableShaderMaterial->constBufferData;

		bool normalMapEnabledChanged, normalMapHasAlphaChanged, specularMapEnabledChanged, specularPowerChanged, specularColorChanged, specularMapWeightChanged, materialColorChanged;

		if (bindableShaderMaterial->constBufferData.ElementExists("normalMapEnabled"))
			normalMapEnabledChanged = ImGui::Checkbox("normalMapEnabled", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled"));
		if (bindableShaderMaterial->constBufferData.ElementExists("normalMapHasAlpha"))
			normalMapHasAlphaChanged = ImGui::Checkbox("normalMapHasAlpha", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapHasAlpha"));
		if (bindableShaderMaterial->constBufferData.ElementExists("specularMapEnabled"))
			normalMapHasAlphaChanged = ImGui::Checkbox("specularMapEnabled", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("specularMapEnabled"));
		if (bindableShaderMaterial->constBufferData.ElementExists("specularPowerChanged"))
			normalMapHasAlphaChanged = ImGui::SliderFloat("specularPowerChanged", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularPowerChanged"), 0.0f, 1000.0f, "%f");
		if (bindableShaderMaterial->constBufferData.ElementExists("specularColor"))
			normalMapHasAlphaChanged = ImGui::ColorPicker4("specularColor", reinterpret_cast<float*>(shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("specularColor")));
		if (bindableShaderMaterial->constBufferData.ElementExists("specularMapWeight"))
			normalMapHasAlphaChanged = ImGui::SliderFloat("specularMapWeight", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularMapWeight"), 0.0f, 2.0f);
		if (bindableShaderMaterial->constBufferData.ElementExists("materialColor"))
			normalMapHasAlphaChanged = ImGui::ColorPicker4("materialColor", reinterpret_cast<float*>(shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("materialColor")));

		bindableShaderMaterial->Update(gfx, shaderMaterial);

		materialsDefined = true;
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
	DynamicConstantBuffer::BufferData shaderMaterial;
	bool materialsDefined = false;

public:
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
};

