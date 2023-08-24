#include "Application.h"
#include "KeyMacros.h"
#include <random>
#include "imgui/imgui.h";

Application::Application(UINT32 width, UINT32 height, const char* name)
	: m_width(width), m_height(height), m_name(name), window(width, height, name), pointLight(window.Graphics)
{
	float aspectRatioX, AspectRatioY;
	if (width > height)
	{	
		aspectRatioX = (float)width / (float)height;
		AspectRatioY = 1.0f;	
	}
	else 
	{	
		AspectRatioY = (float)height / (float)width;
		aspectRatioX = 1.0f;
	}
 	window.Graphics.camera.SetProjection(DirectX::XMMatrixPerspectiveLH(aspectRatioX, AspectRatioY, 0.5f, 40.0f));
}

BOOL Application::Initiate()
{
 	window.Input.Key.allowRepeating(TRUE);

	while (true)
	{
		BOOL result = this->window.ProcessMessage();

		if (result != TRUE)
			return result;

 		this->DoFrame();
 	}
}
void Application::DoFrame()
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

	FLOAT DeltaTime = timer.Mark();

	DirectX::XMFLOAT3 movingDir = {};
	if (window.Input.Key.GetKeyState(KEY_W))
		movingDir.z += DeltaTime;
	if (window.Input.Key.GetKeyState(KEY_S))
		movingDir.z -= DeltaTime;
	if (window.Input.Key.GetKeyState(KEY_E))
		movingDir.x += DeltaTime;
	if (window.Input.Key.GetKeyState(KEY_A))
		movingDir.x -= DeltaTime;
	if (window.Input.Key.GetKeyState(VK_SPACE))
		movingDir.y += DeltaTime;
	if (window.Input.Key.GetKeyState(VK_CONTROL))
		movingDir.y -= DeltaTime;

	if (movingDir.x != 0 || movingDir.y != 0 || movingDir.z != 0)
		window.Graphics.camera.Move(movingDir);

	Vector2int lookOffset = window.Input.Mouse.GetRawInputPos();

	if(cursorLocked && !cursorShowing)
		if (lookOffset.x != 0 || lookOffset.y != 0)
			window.Graphics.camera.Look({ (float)lookOffset.x, (float)lookOffset.y, 0.0f });

	window.Graphics.BeginFrame({ 0,0,0,1 });

	pointLight.Bind(window.Graphics, window.Graphics.camera.GetCamera());

	model_.Draw(window.Graphics);
	modelSheet.Draw(window.Graphics, DeltaTime);
	pointLight.Draw(window.Graphics);

	if (window.Input.Key.GetKeyDown(VK_INSERT))
		window.Graphics.ShowImGUI(!window.Graphics.isImGUIVisible());

	if (window.Input.Key.GetKeyDown(VK_ESCAPE))
	{
		cursorLocked = cursorShowing;
		cursorShowing = !cursorShowing;
		window.LockCursor(cursorLocked);
		window.ShowCursor(cursorShowing);
	}

	window.Graphics.camera.CreateControlMenu();
	pointLight.SpawnControlWindow(window.Graphics);
	modelSheet.SpawnControlWindow(window.Graphics);
	model_.SpawnControlWindow();

	window.Graphics.FinishFrame();
}