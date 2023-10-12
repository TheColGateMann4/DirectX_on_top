#pragma once
#include "Includes.h"
#include "Model.h"
#include "Graphics.h"
#include "Window.h"

class ModelHierarchy
{
public:
	ModelHierarchy() = delete;

	ModelHierarchy(Window* window)
		: m_window(window)	{}

public:
	void DrawModels();
	void DrawModelHierarchy();

private:
	void CleanupPressedNodes();

public:
	std::vector<std::unique_ptr<Model>> models;

private:
	Window* m_window = nullptr;
	Node* m_pressedNode;
};

