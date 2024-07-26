#pragma  once
#include "Includes.h"
#include "Window.h"
#include "Time.h"
#include "ImguiManager.h"
#include "PointLight.h"
#include "Model.h"
#include "Cube.h"
#include "Scene.h"
#include "GaussBlurRenderGraph.h"
#include "FPSCounter.h"

class Application
{
public:
	Application(UINT32 width, UINT32 height, const char* name);
	BOOL Initiate();

public:
	ImguiManager imguiManager;
	Window window;

private:
	void Update();

private:
	UINT32 m_width;
	UINT32 m_height;
	const char* m_name;

	bool cursorLocked = false;
	bool cursorShowing = true;
	bool imguiDemoWindow = false;
	bool showImguiWindows = false;

private:
	Scene scene;
	GaussBlurRenderGraph renderGraph;
	Time timer;
	FPSCounter fpsCounter;

	//camera preview stuff
	std::shared_ptr<RenderTargetWithTexture> m_cameraPreviewTexture;
	std::shared_ptr<RenderTarget> m_cameraPreviewRenderTarget;
	std::shared_ptr<DepthStencilView> m_cameraPreviewDepthStencil;
};

