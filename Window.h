#pragma once
#include "Includes.h"
#include "ErrorHandling.h"
#include "Graphics.h"
#include "Input.h"

class Window
{
public:
	Window(UINT32 width, UINT32 height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator= (const Window&) = delete;

public:
	HWND GetHWnd() const noexcept;
	BOOL ProcessMessage();
	void ShowCursor(bool show);
	void LockCursor(bool lock);

	void RegisterPrntScrHotKey();
	void UnRegisterPrntScrHotKey();

public:
	BOOL OpenFileExplorer(std::string* filename);
	BOOL MultiselectToFilePaths(std::string* multiSelectStr, std::vector<std::string>* filePaths);

public:
	UINT32 GetWidth() const;
	UINT32 GetHeight() const;

private:
	class WindowClass 
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

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
	UINT32 m_width;
	UINT32 m_height;
	HWND m_hWnd;
	BOOL m_cursorShowing = true;
	BOOL m_cursorLocked = false;
	std::vector<char> m_rawInputDataBuffer = {};

public:
	InputSystem Input;
	GFX Graphics;
};