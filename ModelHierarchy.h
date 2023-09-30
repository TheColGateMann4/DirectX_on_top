#pragma once
#include "Includes.h"
#include "Model.h"
#include "Graphics.h"
#include "Window.h"

class ModelHierarchy
{
public:
	void DrawModels(GFX& gfx);
	void DrawModelHierarchy(GFX& gfx);

private:
	void CleanupPressedNodes();

public:
	std::vector<std::unique_ptr<Model>> models;

private:
	Window* m_window;
	Node* m_pressedNode;
};

