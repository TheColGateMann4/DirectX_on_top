#pragma once
#include "Includes.h"
#include "CameraManager.h"

class GFX;
class Window;
class SceneObject;
class PointLight;

class Scene
{
public:
	Scene(Window* window);

public:
	void LinkModelsToPipeline(class RenderGraph& renderGraph);

public:
	void UpdateSceneVisibility(GFX& gfx);
	
	void UpdateModels(GFX& gfx, float deltaTime);

	void BindLights(GFX& gfx);

	void DrawModels(GFX& gfx);

	void DrawTempModels(GFX& gfx);

	void DrawModelHierarchy(bool show);

	void AddSceneObject(std::unique_ptr<SceneObject>&& model);

	CameraManager* GetCameraManager();

	std::vector<PointLight*>& GetLights();

	UINT32 GetHighestIndex();

private:
	void AddLightObject(PointLight* model);

	void AddCameraObject(Camera* model);

private:
	void CleanupPressedNodes();

public:
	std::shared_ptr<class SceneVisibilityManager> m_sceneVisibilityManager;
	std::vector<UINT8> objectValidity;

	std::vector<std::unique_ptr<SceneObject>> m_models;
	std::vector<SceneObject*> m_pRegularModels;
	std::vector<SceneObject*> m_pTempModels;
	std::vector<PointLight*> m_lights;

	Window* m_window;
	SceneObject* m_pressedNode = nullptr;
	CameraManager m_cameraManager;
	INT32 m_highestSceneIndex = -1;// -1 means no objects on scene
};

