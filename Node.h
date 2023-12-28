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
	void Render(RenderQueue& renderQueue, DirectX::XMMATRIX transform) const noexcept(!IS_DEBUG)
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
			mesh->Render(renderQueue, finalTransform);
		}

		//passing accumulated transform to objects lower in hierarchy
		for (const auto& pChild : m_pChildrens)
		{
			pChild->Render(renderQueue, finalTransform);
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

		CachedBuffer* bindableShaderMaterial = m_pMeshes.front()->GetBindable<CachedBuffer>(0,0,1, true);

		if (!m_materialsDefined)
			shaderMaterial = bindableShaderMaterial->constBufferData;

		bool changed = false;

		auto checkChanged = [&changed](bool returnFromStatement) mutable
		{
			changed = changed || returnFromStatement;
		};


		if (bindableShaderMaterial->constBufferData.ElementExists("normalMapEnabled"))
			checkChanged(ImGui::Checkbox("normalMapEnabled", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled")));
		if (bindableShaderMaterial->constBufferData.ElementExists("normalMapHasAlpha"))
			checkChanged(ImGui::Checkbox("normalMapHasAlpha", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapHasAlpha")));
		if (bindableShaderMaterial->constBufferData.ElementExists("specularMapEnabled"))
			checkChanged(ImGui::Checkbox("specularMapEnabled", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("specularMapEnabled")));
		if (bindableShaderMaterial->constBufferData.ElementExists("specularPowerChanged"))
			checkChanged(ImGui::SliderFloat("specularPowerChanged", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularPowerChanged"), 0.0f, 1000.0f, "%f"));
		if (bindableShaderMaterial->constBufferData.ElementExists("specularColor"))
			checkChanged(ImGui::ColorPicker4("specularColor", reinterpret_cast<float*>(shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("specularColor"))));
		if (bindableShaderMaterial->constBufferData.ElementExists("specularMapWeight"))
			checkChanged(ImGui::SliderFloat("specularMapWeight", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularMapWeight"), 0.0f, 2.0f));
		if (bindableShaderMaterial->constBufferData.ElementExists("materialColor"))
			checkChanged(ImGui::ColorPicker4("materialColor", reinterpret_cast<float*>(shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("materialColor"))));

		if(changed)
			bindableShaderMaterial->Update(gfx, shaderMaterial);

		m_materialsDefined = true;
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
	bool m_materialsDefined = false;

public:
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
};

