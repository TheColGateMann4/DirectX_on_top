#pragma once
#include "Includes.h"
#include "CameraManager.h"

class Window;
class SceneObject;

class Scene
{
public:
	Scene(Window* window);

public:
	void LinkModelsToPipeline(class RenderGraph& renderGraph);

public:
	void DrawModels(class GFX& gfx);
	void DrawModelHierarchy(float deltaTime);

	CameraManager* GetCameraManager();

	void AddCameraObject(std::unique_ptr<Camera>&& model);

	void AddSceneObject(std::unique_ptr<SceneObject>&& model);

private:
	void CleanupPressedNodes();

private:
	std::vector<std::unique_ptr<SceneObject>> m_models;

	Window* m_window;
	SceneObject* m_pressedNode = nullptr;
	CameraManager m_cameraManager;
};

