#pragma once
#include "Includes.h"

class GFX;
class Shape;
class RenderGraph;
class ShaderUnorderedAccessView;

class SceneObject
{
	friend class Scene;

public:
	SceneObject(DirectX::XMFLOAT3 startingPosition = {0.0f, 0.0f, 0.0f});

	virtual ~SceneObject() = default;

public:
	virtual void LinkSceneObjectToPipeline(RenderGraph& renderGraph) = 0;

	void LinkChildrenToPipeline(RenderGraph& renderGraph);

public:
	virtual void Update(GFX& gfx, float deltatime);

	virtual void AddChild(std::unique_ptr<SceneObject> child);

	void RenderOnScene() const noexcept(!IS_DEBUG);

	void RenderChildrenOnScene() const noexcept(!IS_DEBUG);

	virtual void RenderThisObjectOnScene() const noexcept(!IS_DEBUG) = 0;

	virtual void MakeHierarchy(GFX& gfx);

	virtual void MakeTransformPropeties(GFX& gfx);

	virtual void MakePropeties(GFX& gfx);

	virtual void MakeAdditionalPropeties(GFX& gfx);

	void SetVisibilityInHierarchy(bool visibility);

	bool GetVisibilityInHierarchy() const;

	bool HaveVisibleChildren() const;

	void GenerateTree(SceneObject*& pressedNode);

	void ResetLocalTransform() noexcept;

	virtual DirectX::XMMATRIX GetSceneTranformMatrix() const noexcept;

	virtual void CalculateSceneTranformMatrix(DirectX::XMMATRIX parentTransform = DirectX::XMMatrixIdentity()) noexcept;

	DirectX::XMFLOAT3 GetWorldPosition() const;

	DirectX::XMFLOAT3 GetWorldRotation() const;

	virtual std::string GetOriginalName(bool withStatus) const;

	void SetVisibility(std::vector<UINT8>& visibilityData);

	void PushObjectMatrixToBuffer(std::vector<DirectX::XMMATRIX>& matrixData) const;

	void InitialzeSceneObject(INT32 sceneIndex, size_t repeatingNameIndex, std::vector<UINT8>& validityData, GFX& gfx, ShaderUnorderedAccessView* pModelCubeRWBuffer);

	UINT32 GetNumChildren(bool getChildrenOfChildren) const;



private:
	void m_InitialzeSceneObject(INT32& internalSceneIndex, size_t repeatingNameIndex, std::vector<UINT8>& validityData, GFX& gfx, ShaderUnorderedAccessView* pModelCubeRWBuffer);

	UINT32 m_GetNumberOfChildren(bool firstOneCalled, bool getChildrenOfChildren) const;

protected:
	virtual const char* GetName() const = 0;

	virtual const char* GetNameSpecialStatus() const;

	virtual void SetSceneIndexes(size_t sceneIndex, size_t repeatingNameIndex);

	void GenerateBoundCube(GFX& gfx, ShaderUnorderedAccessView* pModelCubeRWBuffer);

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
	SceneObject* m_parent = nullptr;

public:
	bool m_pressed = false;
	UINT m_sceneIndex = 0;
	size_t m_sceneNameIndex = 0;

protected:
	bool m_visibleInHierarchy = true;

	Shape* m_shape = nullptr;

	SceneObject* m_pressedNode = nullptr;

	std::vector<std::unique_ptr<SceneObject>> m_children;
	DirectX::XMFLOAT4X4 m_transform = {};
};

