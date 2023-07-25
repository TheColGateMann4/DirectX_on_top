#pragma  once
#include "Includes.h"
#include "Window.h"
#include "ErrorHandling.h"
#include "Time.h"
#include "ImguiManager.h"
#include "PointLight.h"
#include "Model.h"

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
	Model model_{ window.Graphics, "Models\\nano_textured\\nanosuit.obj" };
};

