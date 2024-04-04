#include "SceneObject.h"
#include "Shape.h"

void SceneObject::MakeHierarchy(GFX& gfx)
{
	ImGui::Columns(2, nullptr, true);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;
	if (GetPressedState())
		flags |= ImGuiTreeNodeFlags_Selected;

	ImGui::TreeNodeEx(GetOriginalName(true).c_str(), flags);

	if (ImGui::IsItemClicked())
		SetPressedState(!GetPressedState());

	ImGui::TreePop();
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

void SceneObject::MakeAdditionalPropeties(GFX& gfx, float deltaTime)
{

}

void SceneObject::ResetLocalTransform() noexcept
{
	m_position = {};
	m_rotation = {};
}

DirectX::XMMATRIX SceneObject::GetSceneTranformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z) *
		DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
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