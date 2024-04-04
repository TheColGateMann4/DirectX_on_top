#pragma  once
#include "includes.h"

class InputSystem
{
	friend class Window;
	friend class MouseInput;
	friend class KeyInput;

 private:
	static BOOL m_pressedKeysList[255];
	static BOOL m_releasedKeysList[255];
	static BOOL m_keyStateList[255];

 private:
	class KeyInput
	{
		friend class Window;

	 public:
		//KeyPressed Stuff
		BOOL GetKeyDown(UINT32 key);
		BOOL GetKeyUp(UINT32 key);
		BOOL GetKeyState(UINT32 key);
		BOOL anyKeyPressed();

		VOID allowRepeating(BOOL repeat);

		//Char Stuff
		UINT32 GetText(std::string* textout);
		BOOL GetFinishedText(std::string* textout);

		VOID busyWritingText(BOOL busy);

	 private:
		VOID clearList(BOOL* list);

	 private:
		BOOL m_allowRepeating = FALSE;

		std::string m_charBuffer = {};
		BOOL isWritingText = FALSE;
	};


	class MouseInput
	{
		friend class Window;

	 public:

		DirectX::XMUINT2 GetMousePos();
		BOOL GetMouseButtonDown(UINT8 button, DirectX::XMUINT2* position);
		BOOL GetMouseButtonUp(UINT8 button, DirectX::XMUINT2* position);
		INT8 GetMouseWheel();
		DirectX::XMINT2 GetRawInputPos();

	public:
		void HandleRawInput(INT32 x, INT32 y);

	 private:
		DirectX::XMUINT2 m_pressedMouseKeysList[32] = {};
		DirectX::XMUINT2 m_releasedMouseKeysList[32] = {};
		DirectX::XMUINT2 m_position = {};
		DirectX::XMINT2 m_rawInputPosition = {};
		INT8 m_mouseWheelOffset;
		VOID m_MouseButtonChanged(UINT8 button, BOOL pressed, LPARAM lParam);
	};

 public:
	KeyInput Key;
	MouseInput Mouse;
};