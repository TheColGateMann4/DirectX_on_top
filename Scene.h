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
	void UpdateModels(GFX& gfx, float deltaTime);

	void DrawModels(GFX& gfx);

	void DrawModelHierarchy(bool show);

	void AddSceneObject(std::unique_ptr<SceneObject>&& model);

	CameraManager* GetCameraManager();

	std::vector<PointLight*>& GetLights();

private:
	void AddLightObject(PointLight* model);

	void AddCameraObject(Camera* model);

private:
	void CleanupPressedNodes();

public:
	std::vector<std::unique_ptr<SceneObject>> m_models;
	std::vector<PointLight*> m_lights;

	Window* m_window;
	SceneObject* m_pressedNode = nullptr;
	CameraManager m_cameraManager;
};

