#pragma once
#include "Graphics.h"
#include "imgui/imgui.h"

class SceneObject
{
public:
	virtual void Draw(GFX& gfx) const noexcept(!IS_DEBUG) = 0;

	virtual void MakeHierarchy(GFX& gfx)
	{
		ImGui::Columns(2, nullptr, true);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;
		if (GetPressedState())
			flags |= ImGuiTreeNodeFlags_Selected;

		const bool nodeExpanded = ImGui::TreeNodeEx(GetName().c_str(), flags);

		if (ImGui::IsItemClicked())
			SetPressedState(!GetPressedState());

		ImGui::TreePop();
	}

	virtual void MakePropeties(GFX& gfx, float deltaTime) = 0;


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


private:
	bool currentlyPressed = false;


private:
	ImGuiTreeNodeFlags_ nodeFlags = ImGuiTreeNodeFlags_Leaf;
};

