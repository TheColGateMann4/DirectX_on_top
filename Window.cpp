#include "Window.h"
#include "ErrorMacros.h"
#include "KeyMacros.h"
#include "resource.h"
#include "Graphics.h"
#include "imgui/backend/imgui_impl_win32.h"
#include <shlobj_core.h>


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

HWND Window::GetHWnd() const noexcept
{
	return Window::m_hWnd;
}

UINT32 Window::GetWidth() const
{
	return Window::m_width;
}

UINT32 Window::GetHeight() const
{
	return Window::m_height;
}

BOOL Window::ProcessMessage()
{
	this->Input.Key.clearList(this->Input.m_releasedKeysList);
	this->Input.Key.clearList(this->Input.m_pressedKeysList);
	this->Input.Mouse.HandleRawInput(NULL, NULL);

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

void Window::ShowCursor(bool show)
{
	if ((m_cursorShowing == 1) == show)
		return;

	m_cursorShowing = show;

	if(show)
		while (::ShowCursor(show) < 0);
	else
		while (::ShowCursor(show) >= 0);
}

void Window::LockCursor(bool lock)
{
	m_cursorLocked = lock;

	if (lock)
	{
		RECT clientSpace;
		GetClientRect(m_hWnd, &clientSpace);
		MapWindowPoints(m_hWnd, NULL, reinterpret_cast<POINT*>(&clientSpace), 2);
		ClipCursor(&clientSpace);
	}
	else
	{
		ClipCursor(nullptr);
	}
}


BOOL Window::OpenFileExplorer(std::string* filename)
{

	GetCurrentDirectory(MAX_PATH, filename->data());

	std::string myExtension = "Model Files\0.OBJ\0\0";

	OPENFILENAMEA openFileStructure = {};
	openFileStructure.lStructSize = sizeof(openFileStructure);
	openFileStructure.hwndOwner = NULL;
	openFileStructure.lpstrFilter = myExtension.data();
	openFileStructure.nFilterIndex = 1;
	openFileStructure.lpstrFile = filename->data();
	openFileStructure.nMaxFile = MAX_PATH;
	openFileStructure.Flags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_FORCESHOWHIDDEN | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_NOCHANGEDIR;
	openFileStructure.lpstrTitle = "Import New Model";

	return GetOpenFileName(&openFileStructure);
}

BOOL Window::MultiselectToFilePaths(std::string* multiSelectStr, std::vector<std::string>* filePaths)
{
	size_t currentEndPos = multiSelectStr->find('\0');
	size_t lastEndPos = currentEndPos;

	std::string filepath = std::string(multiSelectStr->begin(), multiSelectStr->end() - (multiSelectStr->length() - currentEndPos)); //searching for first \0 that is used to indicate files

	if (currentEndPos + 1 < multiSelectStr->length())
		if (multiSelectStr->at(currentEndPos + 1) == '\0')
		{
// 			if (std::string(multiSelectStr->end() - 4, multiSelectStr->end()) != ".obj")
// 				return 0; // no .obj extension and only one file was selected

			filePaths->push_back(filepath);
			return 1; // just one file was selected
		}

	for (size_t i = 0; i < multiSelectStr->length();i++)
	{
		currentEndPos = multiSelectStr->find('\0', currentEndPos + 1);
		if (currentEndPos == std::string::npos || currentEndPos == lastEndPos + 1)
			break;

		std::string fileNameAndExtension = std::string(multiSelectStr->begin() + lastEndPos, multiSelectStr->end() - (multiSelectStr->length() - currentEndPos));
		
		if(std::string(fileNameAndExtension.end() - 4, fileNameAndExtension.end()) == ".obj") // doing check for at least 4 characters is pointless, because if a user manages to do this he is weird
			filePaths->push_back(filepath + '\\' + fileNameAndExtension);


		lastEndPos = currentEndPos;
	}

	return 2; // multiple files were selected
}



//! Window

Window::Window(UINT32 width, UINT32 height, const char* name)
	: m_height(height), m_width(width)
{
	RECT rWindow = { NULL };

	rWindow.left = 100;
	rWindow.right = width + rWindow.left;

	rWindow.top = 100;
	rWindow.bottom = height + rWindow.top;

	if (AdjustWindowRect(&rWindow, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
		THROW_LAST_ERROR;

	this->m_hWnd = CreateWindowA(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, // XY
		rWindow.right - rWindow.left, rWindow.bottom - rWindow.top, //size WH
		NULL,
		NULL,
		WindowClass::GetInstance(),
		this
	);
	if (this->m_hWnd == NULL)
		THROW_LAST_ERROR;

	this->Graphics.SetResolution(width, height);
	this->Graphics.Initialize(m_hWnd);

	ShowWindow(m_hWnd, SW_SHOW);

	//initializing imgui
	ImGui_ImplWin32_Init(m_hWnd);

	//registering mouse raw input device
	RAWINPUTDEVICE rawInputDevice = {};
	rawInputDevice.usUsagePage = 0x01; // HID_USAGE_PAGE_GENERIC
	rawInputDevice.usUsage = 0x02; // HID_USAGE_GENERIC_MOUSE
	rawInputDevice.dwFlags = 0;
	rawInputDevice.hwndTarget = m_hWnd;

	if (!RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)))
		THROW_LAST_ERROR;
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(m_hWnd);
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

	ImGuiIO imguiio = ImGui::GetIO();

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
		if (imguiio.WantCaptureKeyboard)
			break;
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
			if (imguiio.WantCaptureKeyboard)
				break;

			this->Input.Key.m_charBuffer += static_cast<char>(wParam);
			break;
		}

	///*          Mouse stuff          *///
	case WM_MOUSEMOVE:
		{
			DirectX::XMUINT2 mousePos = { (UINT32)(short)LOWORD(lParam) ,(UINT32)(short)HIWORD(lParam) };
			if (mousePos.x < this->m_width && mousePos.x >= 0 && mousePos.y < this->m_height && mousePos.y >= 0)
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
			this->Input.Mouse.m_pressedMouseKeysList[KEY_SCROLLWHEEL].x = ((UINT32)(short)LOWORD(lParam));
			this->Input.Mouse.m_pressedMouseKeysList[KEY_SCROLLWHEEL].x = ((UINT32)(short)HIWORD(lParam));

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
			if (m_cursorShowing)
				ShowCursor(true);
			if (m_cursorLocked)
				LockCursor(true);

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
	case WM_INPUT:
		{
			UINT32 size = 0;

			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				nullptr,
				&size,
				sizeof(RAWINPUTHEADER)) == -1) 
			{
				break;
			};

			m_rawInputDataBuffer.resize(size);

			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				m_rawInputDataBuffer.data(),
				&size,
				sizeof(RAWINPUTHEADER)) == -1)
			{
				break;
			};

			const RAWINPUT& rawInput = reinterpret_cast<const RAWINPUT&>(*m_rawInputDataBuffer.data());

			if (rawInput.header.dwType == RIM_TYPEMOUSE &&
				(rawInput.data.mouse.lLastX != 0 || rawInput.data.mouse.lLastY != 0))
			{
				this->Input.Mouse.HandleRawInput(rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY);
			}
		}
	}


	return DefWindowProcA(hWnd, msg, wParam, lParam);
}