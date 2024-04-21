#include "Application.h"
#include "KeyMacros.h"
#include "imgui/imgui.h"
#include <random>
#include "CameraManager.h"
#include "Camera.h"
#include "NormalRenderPass.h"
#include "Skybox.h"

Application::Application(UINT32 width, UINT32 height, const char* name)
	: m_width(width), m_height(height), m_name(name), window(width, height, name), scene(&window), renderGraph(window.Graphics, scene), fpsCounter(window.Graphics)
{
	window.Graphics.LinkCameraManager(scene.GetCameraManager());
}

BOOL Application::Initiate()
{
 	window.Input.Key.allowRepeating(TRUE);

	//scene.AddSceneObject(std::make_unique<Model>(window.Graphics, "Models\\brickwall\\brick_wall.obj", 6.0f));
	//scene.AddSceneObject(std::make_unique<Model>(window.Graphics, "Models\\muro\\muro.obj", 3.0f));

	scene.AddSceneObject(std::make_unique<Camera>(window.Graphics, DirectX::XMFLOAT3{0.0f, 5.0f, -10.0f}));
	//scene.AddSceneObject(std::make_unique<Camera>(window.Graphics));
	scene.AddSceneObject(std::make_unique<PointLight>(window.Graphics, 0.5f, DirectX::XMFLOAT3{3.0f, 2.0f, 0.0f}));
	//scene.AddSceneObject(std::make_unique<Model>(window.Graphics, "Models\\Sponza\\sponza.obj", 1.0f / 20.0f));
	//scene.AddSceneObject(std::make_unique<Model>(window.Graphics, "Models\\Flashlight\\Flashlight.obj", 1.0f, DirectX::XMFLOAT3{ 0.0f, 2.0f, -1.0f }));
	//scene.AddSceneObject(std::make_unique<Model>(window.Graphics, "Models\\muro\\muro.obj", 1.0f, DirectX::XMFLOAT3{ -5.0f, 2.0f, 0.0f }));
	//scene.AddSceneObject(std::make_unique<Model>(window.Graphics, "Models\\Ghosts\\GroundCape1.obj", 1.0f, DirectX::XMFLOAT3{ 7.0f, 2.0f, 0.0f }));
	scene.AddSceneObject(std::make_unique<Cube>(window.Graphics, 1.0f, "Models\\brickwall\\brick_wall_diffuse.jpg", "Models\\brickwall\\brick_wall_normal.jpg", DirectX::XMFLOAT3{ -3.0f, 2.0f, -3.0f }));
	scene.AddSceneObject(std::make_unique<Cube>(window.Graphics, 1.0f, "Models\\brickwall\\brick_wall_diffuse.jpg", "Models\\brickwall\\brick_wall_normal.jpg", DirectX::XMFLOAT3{ -5.0f, 4.0f, -3.0f }));
	scene.AddSceneObject(std::make_unique<Skybox>(window.Graphics, 20.0f, "Images\\SpaceSkybox\\Space.png"));
	scene.LinkModelsToPipeline(renderGraph);

	timer.SetTime();

	while (true)
	{
		BOOL result = this->window.ProcessMessage();

		if (result != TRUE)
			return result;

 		this->Update();
 	}
}
void Application::Update()
{
		// KeyPressedDown / KeyPressedUp
		/*
		window.Input.Key.allowRepeating(FALSE);

		if (window.Input.Key.GetKeyDown(KEY_W))
		{
			MessageBoxA(NULL, "Down", "Dominiczek Pies", MB_OK);
		}
		if (window.Input.Key.GetKeyUp(KEY_W))
		{
			MessageBoxA(NULL, "Up", "Dominiczek Pies", MB_OK);
		}
		*/

		// GetKeyState
		/*
		if (window.Input.Key.GetKeyState(KEY_W))
		{
			OutputDebugStringA("1");
		}
		*/

		// anyKeyPressed
		/*
		if (window.Input.Key.anyKeyPressed())
		{
			OutputDebugStringA("1");
		}
		*/

		// GetText
		/*
		if (window.Input.Key.GetText(NULL) > 0)
		{
			std::string text;
			window.Input.Key.GetText(&text);
			OutputDebugStringA(text.data());
		}
		*/

		// GetFinishedText
		/*
		window.Input.Key.busyWritingText(TRUE);
		if (window.Input.Key.GetFinishedText(NULL))
		{
			std::string text;
			window.Input.Key.GetFinishedText(&text);
			OutputDebugStringA(text.data());
		}
		*/

		// GetMousePos
		/*
		std::string position = std::to_string(window.Input.Mouse.GetMousePos().x) + "  " + std::to_string(window.Input.Mouse.GetMousePos().y) + '\n';
		OutputDebugStringA(position.data());
		*/
		
		// GetMouseButtonDown / GetMouseButtonUp
		/*
		std::string text;
		Vector2i pos;

		if (window.Input.Mouse.GetMouseButtonDown(KEY_LMOUSEBUTTON, &pos))
		{
			text = "Down: " + std::to_string(pos.x) + "  " + std::to_string(pos.y) + "\n";//+ std::to_string(pos.x) + "  " + std::to_string(pos.y) + "\n";
			OutputDebugStringA(text.data());
		}

		if (window.Input.Mouse.GetMouseButtonUp(KEY_LMOUSEBUTTON, &pos))
		{
			text = "Up: " + std::to_string(pos.x) + "  " + std::to_string(pos.y) + "\n";//+ std::to_string(pos.x) + "  " + std::to_string(pos.y) + "\n";
			OutputDebugStringA(text.data());
		}
		*/

	float deltaTime = timer.GetDeltaTime(true);

	DirectX::XMFLOAT3 movingDir = {};
	if (window.Input.Key.GetKeyState(KEY_W))
		movingDir.z += deltaTime;
	if (window.Input.Key.GetKeyState(KEY_S))
		movingDir.z -= deltaTime;
	if (window.Input.Key.GetKeyState(KEY_E))
		movingDir.x += deltaTime;
	if (window.Input.Key.GetKeyState(KEY_A))
		movingDir.x -= deltaTime;
	if (window.Input.Key.GetKeyState(VK_SPACE))
		movingDir.y += deltaTime;
	if (window.Input.Key.GetKeyState(VK_CONTROL))
		movingDir.y -= deltaTime;

	if (movingDir.x != 0 || movingDir.y != 0 || movingDir.z != 0)
		scene.GetCameraManager()->GetActiveCamera()->Move(movingDir);

	DirectX::XMINT2 lookOffset = window.Input.Mouse.GetRawInputPos();

	if (cursorLocked && !cursorShowing)
		if (lookOffset.x != 0 || lookOffset.y != 0)
			scene.GetCameraManager()->GetActiveCamera()->Look({ (float)lookOffset.x, (float)lookOffset.y, 0.0f }, true);

	window.Graphics.BeginFrame();

	scene.UpdateModels(window.Graphics, deltaTime);

	scene.DrawModels(window.Graphics);

	renderGraph.Render(window.Graphics);

	if (window.Input.Key.GetKeyDown(VK_INSERT))
		showImguiWindows = !showImguiWindows;

	if (window.Input.Key.GetKeyDown(VK_ESCAPE))
	{
		cursorLocked = cursorShowing;
		cursorShowing = !cursorShowing;
		window.LockCursor(cursorLocked);
		window.ShowCursor(cursorShowing);
	}

	if (window.Input.Key.GetKeyDown(VK_SNAPSHOT)) // print screen key
		renderGraph.CaptureNextFrame();

	if(window.Input.Key.GetKeyDown(VK_OEM_3)) // tilde key
		imguiDemoWindow = !imguiDemoWindow;

	if(showImguiWindows && imguiDemoWindow)
		ImGui::ShowDemoWindow();

	static_cast<NormalRenderPass*>(renderGraph.GetRenderJobPass("normalPass"))->ShowWindow(window.Graphics, showImguiWindows);

	scene.DrawModelHierarchy(showImguiWindows);

	fpsCounter.Draw(deltaTime);

	window.Graphics.FinishFrame();

	renderGraph.Reset();
}