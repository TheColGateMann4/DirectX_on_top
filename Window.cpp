#include "Window.h"
#include "ErrorMacros.h"
#include "KeyMacros.h"
#include "resource.h"
#include "imgui/backend/imgui_impl_win32.h"
//! WindowClass

Window::WindowClass Window::WindowClass::sWindowClass;

Window::WindowClass::WindowClass() noexcept
	: hInstance(GetModuleHandle(NULL))
{
	WNDCLASSEXA windowClass = { NULL };
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = HandleStartMessage;
	windowClass.cbClsExtra = NULL;
	windowClass.cbWndExtra = NULL;
	windowClass.hInstance = Window::WindowClass::hInstance;
	windowClass.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 96, 96, NULL));
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = Window::WindowClass::sClassName;
	windowClass.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 96, 96, NULL));;

	RegisterClassExA(&windowClass);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClassA(Window::WindowClass::sClassName, Window::WindowClass::GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return Window::WindowClass::sClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return Window::WindowClass::sWindowClass.hInstance;
}

HWND Window::GetHWnd() noexcept
{
	return Window::shWnd;
}

UINT32 Window::GetWidth()
{
	return Window::sWidth;
}

UINT32 Window::GetHeight()
{
	return Window::sHeight;
}

BOOL Window::ProcessMessage()
{
	this->Input.Key.clearList(this->Input.m_releasedKeysList); //i know its weird, but i really want to use those and have it work only once
	this->Input.Key.clearList(this->Input.m_pressedKeysList);

	MSG msg;

	while (PeekMessageA(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return static_cast<BOOL>(msg.wParam);
		}

		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return TRUE;
}
//! Window

Window::Window(UINT32 width, UINT32 height, const char* name)
	: sHeight(height), sWidth(width)
{
	RECT rWindow = { NULL };

	rWindow.left = 100;
	rWindow.right = width + rWindow.left;

	rWindow.top = 100;
	rWindow.bottom = height + rWindow.top;

	if (AdjustWindowRect(&rWindow, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
		THROW_LAST_ERROR;

	this->shWnd = CreateWindowA(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, // XY
		rWindow.right - rWindow.left, rWindow.bottom - rWindow.top, //size WH
		NULL,
		NULL,
		WindowClass::GetInstance(),
		this
	);
	if (this->shWnd == NULL)
		THROW_LAST_ERROR;

	this->Graphics.SetResolution(width, height);
	this->Graphics.Initialize(shWnd);

	ShowWindow(shWnd, SW_SHOW);

	ImGui_ImplWin32_Init(shWnd);
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(shWnd);
}

LRESULT WINAPI Window::HandleStartMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_CREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		SetWindowLongPtrA( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

		SetWindowLongPtrA(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::MessageHub));

		return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
	}

	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::MessageHub(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* pWnd = reinterpret_cast<Window*>(GetWindowLongPtrA(hWnd, GWLP_USERDATA));

	return pWnd->HandleMessage(hWnd, msg, wParam,lParam);
}

LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	///*          Window Behaving stuff          *///
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_KILLFOCUS:
	{
		//clearing keyboard pressed keys to avoid weird behaviors
		
		this->Input.Key.clearList(this->Input.m_pressedKeysList);
		this->Input.Key.clearList(this->Input.m_releasedKeysList);
		this->Input.Key.clearList(this->Input.m_keyStateList);

		break;
	}


	///*          Keyboard stuff          *///
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
		if (!this->Input.Key.isWritingText)
			if (lParam & 0x40000000) //was pressed before
				if (this->Input.Key.m_allowRepeating)
				{
					this->Input.m_pressedKeysList[wParam] = TRUE;
					this->Input.m_keyStateList[wParam] = TRUE;
				}
				else
				{
					this->Input.m_pressedKeysList[wParam] = FALSE;
					this->Input.m_keyStateList[wParam] = FALSE;
				}
			else
			{
				this->Input.m_pressedKeysList[wParam] = TRUE;
				this->Input.m_keyStateList[wParam] = TRUE;
			}
			else 
				if (wParam == VK_RETURN)
					this->Input.Key.m_charBuffer += '\n'; //because WM_CHAR doesn't have '\n' in it, I used it for more convenient use of text

			break;
		}

	case WM_SYSKEYUP:
	case WM_KEYUP:
		{
			this->Input.m_keyStateList[wParam] = FALSE;
			this->Input.m_releasedKeysList[wParam] = TRUE;

			break;
		}

	case WM_SYSCHAR:
	case WM_CHAR:
		{
			this->Input.Key.m_charBuffer += static_cast<char>(wParam);
			break;
		}

		///*          Mouse stuff          *///
	case WM_MOUSEMOVE:
		{
			Vector2i mousePos = { (UINT32)(short)LOWORD(lParam) ,(UINT32)(short)HIWORD(lParam) };
			if (mousePos.x < this->sWidth && mousePos.x >= 0 && mousePos.y < this->sHeight && mousePos.y >= 0)
			{
				this->Input.Mouse.m_position.x = mousePos.x;
				this->Input.Mouse.m_position.y = mousePos.y;
			}
			/*else 
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					this->Input.Mouse.m_position.x = mousePos.x;
					this->Input.Mouse.m_position.y = mousePos.y;
				}
			}*/
			break;
		}
	case WM_MOUSEWHEEL:
		{
			this->Input.Mouse.m_pressedKeysList[KEY_SCROLLWHEEL].x = ((UINT32)(short)LOWORD(lParam));
			this->Input.Mouse.m_pressedKeysList[KEY_SCROLLWHEEL].x = ((UINT32)(short)HIWORD(lParam));

			this->Input.Mouse.m_mouseWheelOffset = (UINT8)GET_WHEEL_DELTA_WPARAM(wParam);
			
			break;
		}

	case WM_RBUTTONDOWN:
		{
			this->Input.Mouse.m_MouseButtonChanged(KEY_RMOUSEBUTTON, TRUE, lParam);
			break;
		}
	case WM_RBUTTONUP:
		{
			this->Input.Mouse.m_MouseButtonChanged(KEY_RMOUSEBUTTON, FALSE, lParam);
			break;
		}

	case WM_LBUTTONDOWN:
		{
			this->Input.Mouse.m_MouseButtonChanged(KEY_LMOUSEBUTTON, TRUE, lParam);
			break;
		}
	case WM_LBUTTONUP:
		{
			this->Input.Mouse.m_MouseButtonChanged(KEY_LMOUSEBUTTON, FALSE, lParam);
			break;
		}

	case WM_MBUTTONDOWN:
		{
			this->Input.Mouse.m_MouseButtonChanged(KEY_MMOUSEBUTTON, TRUE, lParam);
			break;
		}
	case WM_MBUTTONUP:
		{
			this->Input.Mouse.m_MouseButtonChanged(KEY_MMOUSEBUTTON, FALSE, lParam);
			break;
		}

	}


	return DefWindowProcA(hWnd, msg, wParam, lParam);
}