#pragma once
#include "Includes.h"
#include "Model.h"
#include "Graphics.h"
#include "Window.h"

class Scene
{
public:
	Scene() = delete;

	Scene(Window* window)
		: m_window(window)	{}

public:
	void DrawModels(RenderQueue& renderQueue, GFX& gfx, DirectX::XMMATRIX CameraView_);
	void DrawModelHierarchy(float deltaTime);

private:
	void CleanupPressedNodes();

public:
	std::vector<std::unique_ptr<SceneObject>> models;

private:
	Window* m_window = nullptr;
	SceneObject* m_pressedNode = nullptr;
};

