#pragma  once
#include "Includes.h"
#include "Window.h"
#include "ErrorHandling.h"
#include "Time.h"
#include "ImguiManager.h"
#include "PointLight.h"
#include "Model.h"
#include "Cube.h"
#include "ModelHierarchy.h"

class Application
{
public:
	Application(UINT32 width, UINT32 height, const char* name);
	BOOL Initiate();

public:
	ImguiManager imguiManager;
	Window window;
	Time timer;

private:
	void DoFrame();

private:
	UINT32 m_width;
	UINT32 m_height;
	const char* m_name;

	bool cursorLocked = false;
	bool cursorShowing = true;

	//scene
private:
	PointLight pointLight;
	Cube mycube{ window.Graphics, 1.0f, "Models\\brickwall\\brick_wall_diffuse.jpg", "Models\\brickwall\\brick_wall_normal.jpg"};
	ModelHierarchy modelHierarchy{ &window };
};

