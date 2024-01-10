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

	}

	virtual void MakeAdditionalPropeties(GFX& gfx, float deltaTime)
	{

	}

	void ResetLocalTransform() noexcept
	{
		m_position = {};
		m_rotation = {};
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
	
public:
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };

private:
	bool currentlyPressed = false;

private:
	ImGuiTreeNodeFlags_ nodeFlags = ImGuiTreeNodeFlags_Leaf;
};

