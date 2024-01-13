#pragma once
#include "Graphics.h"
#include "imgui/imgui.h"

class SceneObject
{
public:
	virtual void RenderOnScene(RenderQueue& renderQueue) const noexcept(!IS_DEBUG) = 0;

	virtual void MakeHierarchy(GFX& gfx)
	{
		ImGui::Columns(2, nullptr, true);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;
		if (GetPressedState())
			flags |= ImGuiTreeNodeFlags_Selected;

		ImGui::TreeNodeEx(GetName().c_str(), flags);

		if (ImGui::IsItemClicked())
			SetPressedState(!GetPressedState());

		ImGui::TreePop();
	}

	virtual void MakeTransformPropeties(GFX& gfx)
	{
		if (!GetPressedState())
			return;

		ImGui::Text("Position");
		ImGui::SliderFloat("pX", &m_position.x, -30.0f, 30.0f);
		ImGui::SliderFloat("pY", &m_position.y, -30.0f, 30.0f);
		ImGui::SliderFloat("pZ", &m_position.z, -30.0f, 30.0f);

		ImGui::Text("Rotation");
		ImGui::SliderFloat("rX", &m_rotation.x, -_Pi, _Pi);
		ImGui::SliderFloat("rY", &m_rotation.y, -_Pi, _Pi);
		ImGui::SliderFloat("rZ", &m_rotation.z, -_Pi, _Pi);

		if (ImGui::Button("Reset"))
			ResetLocalTransform();
	}

	virtual void MakePropeties(GFX& gfx)
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

	virtual void MakeAdditionalPropeties(GFX& gfx, float deltaTime)
	{

	}

	void ResetLocalTransform() noexcept
	{
		m_position = {};
		m_rotation = {};
	}

	DirectX::XMMATRIX GetSceneTranformMatrix() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z) *
			DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	}

public:
	virtual std::string GetName() const = 0;


public:
	bool GetPressedState()
	{
		return currentlyPressed;
	}

	void SetPressedState(bool newState)
	{
		currentlyPressed = newState;
	}
	
	void SetShape(Shape* shape) noexcept
	{
		m_shape = shape;
	}

public:
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };

private:
	bool currentlyPressed = false;

private:
	ImGuiTreeNodeFlags_ nodeFlags = ImGuiTreeNodeFlags_Leaf;
	Shape* m_shape = nullptr;
};

