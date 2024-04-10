#pragma once
#include "Includes.h"
#include "CameraManager.h"

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
	void UpdateModels(float deltaTime);

	void DrawModels(class GFX& gfx);

	void DrawModelHierarchy();

	CameraManager* GetCameraManager();

	void AddLightObject(PointLight* model);

	void AddCameraObject(Camera* model);

	void AddSceneObject(std::unique_ptr<SceneObject>&& model);

	std::vector<PointLight*>& GetLights();

private:
	void CleanupPressedNodes();

private:
	std::vector<std::unique_ptr<SceneObject>> m_models;
	std::vector<PointLight*> m_lights;

	Window* m_window;
	SceneObject* m_pressedNode = nullptr;
	CameraManager m_cameraManager;
};

