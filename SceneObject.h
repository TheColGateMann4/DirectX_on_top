#pragma once
#include "Includes.h"
#include "imgui/imgui.h"

class GFX;
class Shape;

class SceneObject
{
public:
	virtual ~SceneObject() = default;

public:
	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) = 0;

public:
	virtual void RenderOnScene() const noexcept(!IS_DEBUG) = 0;

	virtual void MakeHierarchy(GFX& gfx);

	virtual void MakeTransformPropeties(GFX& gfx);

	virtual void MakePropeties(GFX& gfx);

	virtual void MakeAdditionalPropeties(GFX& gfx, float deltaTime);

	void ResetLocalTransform() noexcept;

	DirectX::XMMATRIX GetSceneTranformMatrix() const noexcept;

public:
	virtual const char* GetName() const = 0;

	virtual const char* GetNameSpecialStatus() const;

	virtual std::string GetOriginalName(bool withStatus) const;

	virtual void SetSceneIndex(size_t index);


public:
	bool GetPressedState() const
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
	size_t m_sceneIndex = 0;

private:
	ImGuiTreeNodeFlags_ nodeFlags = ImGuiTreeNodeFlags_Leaf;
	Shape* m_shape = nullptr;
};

