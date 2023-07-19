#pragma once
#include "Includes.h"
#include "Shape.h"
#include "Mesh.h"
#include "imgui/imgui.h"

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
			(DirectX::XMLoadFloat4x4(&m_baseTransform) *													//original node transform
				(DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *									//local added scale to current node
					(DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *		//local added rotation to current node
						DirectX::XMMatrixTranslation(position.x, position.y, position.z)				//local added position to current node
			))) * transform;																			//passed transform from nodes higher in hierarchy

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

		if (ImGui::TreeNodeEx(m_nodeName.c_str(), flags))
		{
			if (ImGui::IsItemClicked())
				if (pressedNode != this)
					pressedNode = this;
				else
					pressedNode = nullptr;

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
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
};

