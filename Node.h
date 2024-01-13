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
				DirectX::XMMatrixTranslation(position.x, position.y, position.z)				//added position
				)
			* DirectX::XMLoadFloat4x4(&m_baseTransform)											// original transform in node
			* transform;																		// accumulated transform from upper nodes

		for (const auto& pMesh : m_pMeshes)
		{
			pMesh->Render(renderQueue, finalTransform);
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

public:
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
};

