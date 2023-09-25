#pragma  once
#include "Includes.h"
#include "Window.h"
#include "ErrorHandling.h"
#include "Time.h"
#include "ImguiManager.h"
#include "PointLight.h"
#include "Model.h"
#include "Sheet.h"

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
	Model modelNano{ window.Graphics, "Models\\nano_textured\\nanosuit.obj", 1.0f };
	Model modelWall{ window.Graphics, "Models\\brickwall\\brick_wall.obj", 6.0f};
	Model modelGoblin{ window.Graphics, "Models\\muro\\muro.obj", 3.0f};
};

