#include "Application.h"
#include "KeyMacros.h"
#include "Sphere.h"
#include "Cube.h"
#include "Pyramid.h"
#include "Sheet.h"
#include <random>
#define KEYMACROAWW(varible, pressedkeyqw, changevalue) 		if (window.Input.Key.GetKeyState(pressedkeyqw)&1) \
																	varible += changevalue;
Application::Application(UINT32 width, UINT32 height, const char* name)
	: m_width(width), m_height(height), m_name(name),
	window(width, height, name)
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist{ 0.0f,(float)std::_Pi * 2.0f };
	std::uniform_real_distribution<float> ddist{ 0.0f,(float)std::_Pi * 0.5f };
	std::uniform_real_distribution<float> odist{ 0.0f,(float)std::_Pi * 0.08f };
	std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
	std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
	std::uniform_int_distribution<UINT32> latdist{ 5,20 };
	std::uniform_int_distribution<UINT32> longdist{ 10,40 };
	std::uniform_int_distribution<UINT32> typedist{ 0,2 };

	for (auto i = 0; i < 1; i++)
	{
		//boxes.push_back(std::make_unique<Sphere>(window.Graphics, rng, adist, ddist, odist, rdist, longdist, latdist));
		//boxes.push_back(std::make_unique<Cube>(window.Graphics, rng, adist, ddist, odist, rdist));
		//boxes.push_back(std::make_unique<Pyramid>(window.Graphics, rng, adist, ddist, odist, rdist));
 		boxes.push_back(std::make_unique<Sheet>(window.Graphics, 10));
	}
 	window.Graphics.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0, 3.0 / 4.0, 0.5, 40.0));
}

BOOL Application::Initiate()
{
 	window.Input.Key.allowRepeating(true);

	while (true)
	{
		
		BOOL result = this->window.ProcessMessage();

		if (result != TRUE)
			return result;


 		this->DoFrame();
 	}
}
VOID Application::DoFrame()
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
	window.Graphics.ClearBuffer({ 0,0,0,1 });
 	for (std::unique_ptr<Sheet>& b : boxes)
 	{
 		KEYMACROAWW(b->r, KEY_A, 0.05f);
 		KEYMACROAWW(b->r, KEY_Z, -0.05f);
 
 		KEYMACROAWW(b->roll, KEY_S, 0.05f);
 		KEYMACROAWW(b->roll, KEY_X, -0.05f);
 
 		KEYMACROAWW(b->pitch, KEY_D, 0.05f);
 		KEYMACROAWW(b->pitch, KEY_C, -0.05f);
 
 		KEYMACROAWW(b->yaw, KEY_F, 0.05f);
 		KEYMACROAWW(b->yaw, KEY_V, -0.05f);
 
 		KEYMACROAWW(b->theta, KEY_G, 0.05f);
 		KEYMACROAWW(b->theta, KEY_B, -0.05f);
 
 		KEYMACROAWW(b->phi, KEY_H, 0.05f);
 		KEYMACROAWW(b->phi, KEY_N, -0.05f);
 
 		KEYMACROAWW(b->chi, KEY_J, 0.05f);
 		KEYMACROAWW(b->chi, KEY_M, -0.005);

 		b->Update(DeltaTime);
 		b->Draw(window.Graphics, timer.Get());
 	}
	window.Graphics.FinishFrame();
}