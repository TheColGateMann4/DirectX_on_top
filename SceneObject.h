#pragma once
#include "Includes.h"

class GFX;
class Shape;
class RenderGraph;

class SceneObject
{
	friend class Scene;

public:
	virtual ~SceneObject() = default;

public:
	virtual void LinkSceneObjectToPipeline(RenderGraph& renderGraph) = 0;

	void LinkChildrenToPipeline(RenderGraph& renderGraph);

public:
	virtual void Update(float deltatime);

	virtual void AddChild(std::unique_ptr<SceneObject> child);

	void RenderOnScene() const noexcept(!IS_DEBUG);

	void RenderChildrenOnScene() const noexcept(!IS_DEBUG);

	virtual void RenderThisObjectOnScene() const noexcept(!IS_DEBUG) = 0;

	virtual void MakeHierarchy(GFX& gfx);

	virtual void MakeTransformPropeties(GFX& gfx);

	virtual void MakePropeties(GFX& gfx);

	virtual void MakeAdditionalPropeties(GFX& gfx);

	void GenerateTree(SceneObject*& pressedNode);

	void ResetLocalTransform() noexcept;

	DirectX::XMMATRIX GetSceneTranformMatrix() const noexcept;

	virtual void CalculateSceneTranformMatrix(DirectX::XMMATRIX parentTransform = DirectX::XMMatrixIdentity()) noexcept;

	DirectX::XMFLOAT3 GetWorldPosition() const;

public:
	virtual const char* GetName() const = 0;

	virtual const char* GetNameSpecialStatus() const;

	virtual std::string GetOriginalName(bool withStatus) const;

	virtual void SetSceneIndex(size_t index);


public:
	bool GetPressedState() const
	{
		return m_pressed;
	}

	void SetPressedState(bool newState)
	{
		m_pressed = newState;
	}
	
	void SetShape(Shape* shape) noexcept
	{
		m_shape = shape;
	}

public:
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };

public:
	bool m_pressed = false;
	size_t m_sceneIndex = 0;

protected:
	Shape* m_shape = nullptr;

	SceneObject* m_pressedNode = nullptr;

	SceneObject* m_parent = nullptr;

	std::vector<std::unique_ptr<SceneObject>> m_children;
	DirectX::XMFLOAT4X4 m_transform = {};
};

