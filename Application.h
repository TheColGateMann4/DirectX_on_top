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

	//scene
private:
	PointLight pointLight;
	Model model_{ window.Graphics, "boxy.gltf" };
};

