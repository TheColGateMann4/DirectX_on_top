#pragma once
#include "Includes.h"
#include "ErrorHandling.h"
#include "Input.h"
#include "Graphics.h"

class Window
{
 public:
	Window(UINT32 width, UINT32 height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator= (const Window&) = delete;

 public:
	HWND GetHWnd() noexcept;
	BOOL ProcessMessage();

public:
	UINT32 GetWidth();
	UINT32 GetHeight();

 private:
	class WindowClass 
	{
	 public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;;

	 private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator= (const WindowClass&) = delete;

		static constexpr const char* sClassName = "DirectX_on_top";
		static WindowClass sWindowClass;
		HINSTANCE hInstance;
	};

 private:
	static LRESULT WINAPI HandleStartMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT WINAPI MessageHub(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

 private:
	UINT32 sWidth;
	UINT32 sHeight;
	HWND shWnd;

 public:
	InputSystem Input;
	GFX Graphics;
};