#include "SceneObject.h"
#include "imgui/imgui.h"
#include "Shape.h"

SceneObject::SceneObject(DirectX::XMFLOAT3 startingPosition)
{
	m_position = startingPosition;
}

void SceneObject::LinkChildrenToPipeline(RenderGraph& renderGraph)
{
	for (auto& child : m_children)
	{
		child->LinkSceneObjectToPipeline(renderGraph);
		child->LinkChildrenToPipeline(renderGraph);
	}
}

void SceneObject::Update(float deltatime)
{

}

void SceneObject::AddChild(std::unique_ptr<SceneObject> child)
{
	child->m_parent = this;

	m_children.push_back(std::move(child));
}

void SceneObject::RenderOnScene() const noexcept(!IS_DEBUG)
{
	this->RenderThisObjectOnScene();

	this->RenderChildrenOnScene();
}

void SceneObject::RenderChildrenOnScene() const noexcept(!IS_DEBUG)
{
	for (const auto& child : m_children)
		child->RenderOnScene();
}

void SceneObject::MakeHierarchy(GFX& gfx)
{
	ImGui::Columns(2, nullptr, true);

	SceneObject* previous = m_pressedNode;

	this->GenerateTree(previous);

	m_pressedNode = previous;
}

void SceneObject::GenerateTree(SceneObject*& pressedNode)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

	if (m_children.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;
	if (m_pressed)
		flags |= ImGuiTreeNodeFlags_Selected;

	const bool nodeExpanded = ImGui::TreeNodeEx(GetOriginalName(true).c_str(), flags);

	if (ImGui::IsItemClicked())
		if (pressedNode != this)
		{
			if (pressedNode != nullptr)
				pressedNode->SetPressedState(false);

			m_pressed = true;
			pressedNode = this;
		}
		else
		{
			m_pressed = false;
			pressedNode = nullptr;
		}

	if (nodeExpanded)
	{
		for (const auto& child : m_children)
			child->GenerateTree(pressedNode);

		ImGui::TreePop();
	}
}

void SceneObject::MakeTransformPropeties(GFX& gfx)
{
	if (!GetPressedState())
		return;

	ImGui::Text("Position");
	ImGui::SliderFloat("pX", &m_position.x, -30.0f, 30.0f);
	ImGui::SliderFloat("pY", &m_position.y, -30.0f, 30.0f);
	ImGui::SliderFloat("pZ", &m_position.z, -30.0f, 30.0f);

	ImGui::Text("Rotation");
	ImGui::SliderAngle("rX", &m_rotation.x, -180.0f, 180.0f);
	ImGui::SliderAngle("rY", &m_rotation.y, -180.0f, 180.0f);
	ImGui::SliderAngle("rZ", &m_rotation.z, -180.0f, 180.0f);

	if (ImGui::Button("Reset"))
		ResetLocalTransform();
}

void SceneObject::MakePropeties(GFX& gfx)
{
	if (m_shape == nullptr || !GetPressedState())
		return;

	for (auto& technique : m_shape->m_techniques)
	{
		// Setting category text so we know what are we working on in inspector
		{
			bool techniqueActive = technique.GetTechniqueActive();

			ImU32 textColor = techniqueActive ? IM_COL32(0, 255, 0, 255) : IM_COL32(128, 128, 128, 255);
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);

			if (ImGui::Checkbox(technique.GetName().c_str(), &techniqueActive))
				technique.SetTechniqueActive(techniqueActive);

			ImGui::PopStyleColor();
		}


		// Writing off constant buffers
		{
			std::vector< std::pair<std::string, std::vector<CachedBuffer*>> > allStepBuffers = {};
			technique.GetEveryBindableOfType<CachedBuffer>(allStepBuffers);

			for (auto& stepBufferData : allStepBuffers)
			{
				// Writing the name of step se we know what we are working on - again
				// 		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
				//		ImGui::Text(stepBufferData.first.c_str());
				//		ImGui::PopStyleColor();

				// Writing off stuff from each buffer to set by user
				for (auto& constantBuffer : stepBufferData.second)
				{
					DynamicConstantBuffer::BufferData bufferData = constantBuffer->constBufferData;
					if (bufferData.MakeImguiMenu())
						constantBuffer->Update(gfx, bufferData);
				}
			}
		}
	}
}

void SceneObject::MakeAdditionalPropeties(GFX& gfx)
{

}

void SceneObject::ResetLocalTransform() noexcept
{
	m_position = {};
	m_rotation = {};
}

DirectX::XMMATRIX SceneObject::GetSceneTranformMatrix() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_transform);
}

void SceneObject::CalculateSceneTranformMatrix(DirectX::XMMATRIX parentTransform) noexcept
{
	const auto finalTransform =
		(
			DirectX::XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, m_rotation.z) *
			DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z)
			)
		* parentTransform;

	for (const auto& pChild : m_children)
	{
		pChild->CalculateSceneTranformMatrix(finalTransform);
	}

	DirectX::XMStoreFloat4x4(&m_transform , finalTransform);
}

DirectX::XMFLOAT3 SceneObject::GetWorldPosition() const
{
	DirectX::XMFLOAT3 result = m_position;
	SceneObject* parent = m_parent;

	// using nasa loop conventions hehe, gotta keep in mind that 128 is parent->child connections
	for(int i = 0; i < 128; i++)
	{
		if (parent == nullptr)
			return result;

		const DirectX::XMVECTOR vecChildPos = XMLoadFloat3(&result);
		const DirectX::XMVECTOR vecResult = XMVector3Transform(vecChildPos, parent->GetSceneTranformMatrix());
		DirectX::XMStoreFloat3(&result, vecResult);

		parent = parent->m_parent;
	}

	return result;
}

DirectX::XMFLOAT3 SceneObject::GetWorldRotation() const
{
	DirectX::XMFLOAT3 result = m_rotation;
	SceneObject* parent = m_parent;

	for (int i = 0; i < 128; i++)
	{
		if (parent == nullptr)
			return result;

		result.x += parent->m_rotation.x;
		result.y += parent->m_rotation.y;
		result.z += parent->m_rotation.z;

		parent = parent->m_parent;
	}

	return result;
}

const char* SceneObject::GetNameSpecialStatus() const
{
	return "";
}

std::string SceneObject::GetOriginalName(bool withStatus) const
{
	std::string result = GetName();

	if (m_sceneIndex > 0)
		result += "_" + std::to_string(m_sceneIndex);

	if(withStatus)
		result.append(GetNameSpecialStatus());

	return result;
}

void SceneObject::SetSceneIndex(size_t index)
{
	m_sceneIndex = index;
}