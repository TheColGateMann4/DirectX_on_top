#include "Input.h"

BOOL InputSystem::m_pressedKeysList[255];
BOOL InputSystem::m_releasedKeysList[255];
BOOL InputSystem::m_keyStateList[255];


/*

	  KEYBOARD INPUT
			||
			||
			\/

*/


VOID InputSystem::KeyInput::clearList(BOOL* list)
{
	memset(list, FALSE, 255);

	return;
}

BOOL InputSystem::KeyInput::GetKeyDown(UINT32 key)
{
	if (key > 254)
		return FALSE;

	return InputSystem::m_pressedKeysList[key];
}

BOOL InputSystem::KeyInput::GetKeyUp(UINT32 key)
{
	if (key > 254)
		return FALSE;

	return InputSystem::m_releasedKeysList[key];
}

BOOL InputSystem::KeyInput::GetKeyState(UINT32 key)
{
	if (key > 254)
		return FALSE;

	return InputSystem::m_keyStateList[key];
}

BOOL InputSystem::KeyInput::anyKeyPressed()
{
	for (UINT8 i = 0; i < 255; i++)
	{
		if (InputSystem::m_keyStateList[i] == TRUE)
			return TRUE;
	}

	return FALSE;
}

VOID InputSystem::KeyInput::allowRepeating(BOOL repeat)
{
	m_allowRepeating = repeat;

	return;
}

UINT32 InputSystem::KeyInput::GetText(std::string* textout)
{
	UINT32 textLen = (UINT32)InputSystem::KeyInput::m_charBuffer.length();

	if (textout != NULL)
	{
		*textout = InputSystem::KeyInput::m_charBuffer;

		InputSystem::KeyInput::m_charBuffer.clear();
	}

	return textLen;
}

BOOL InputSystem::KeyInput::GetFinishedText(std::string* textout)
{
	UINT32 crPosition = (UINT32)InputSystem::KeyInput::m_charBuffer.find('\n');

	if (crPosition == std::string::npos)
		return FALSE;

	if (textout != nullptr)
	{
		//strncpy_s(textout->data(), crPosition, InputSystem::KeyInput::m_charBuffer.data());
		//strncpy_s<std::string>(textout->data(), InputSystem::KeyInput::m_charBuffer.data(), 32)
		textout->assign(std::string(m_charBuffer.begin(), m_charBuffer.begin() + crPosition));
		//memcpy(&textout, &(InputSystem::KeyInput::m_charBuffer), crPosition + 1);

		InputSystem::KeyInput::m_charBuffer.clear();

	}

	return TRUE;
}

VOID InputSystem::KeyInput::busyWritingText(BOOL busy)
{
	InputSystem::KeyInput::isWritingText = busy;

	return;
}



/*

		MOUSE INPUT
			||
			||
			\/

*/

DirectX::XMUINT2 InputSystem::MouseInput::GetMousePos()
{
	return InputSystem::MouseInput::m_position;
}

BOOL InputSystem::MouseInput::GetMouseButtonDown(UINT8 button, DirectX::XMUINT2* position)
{
	if (button > 31)
		return FALSE;


	*position = InputSystem::MouseInput::m_pressedKeysList[button];

	return InputSystem::m_pressedKeysList[button];
}

BOOL InputSystem::MouseInput::GetMouseButtonUp(UINT8 button, DirectX::XMUINT2* position)
{
	if (button > 31)
		return FALSE;


	*position = InputSystem::MouseInput::m_releasedKeysList[button];

	return InputSystem::m_releasedKeysList[button];
}

INT8 InputSystem::MouseInput::GetMouseWheel()
{
	return InputSystem::MouseInput::m_mouseWheelOffset;
}

DirectX::XMINT2 InputSystem::MouseInput::GetRawInputPos()
{
	return m_rawInputPosition;
}

void InputSystem::MouseInput::HandleRawInput(INT32 mouseRawX, INT32 mouseRawY)
{
	m_rawInputPosition = { mouseRawX,mouseRawY };
}

VOID InputSystem::MouseInput::m_MouseButtonChanged(UINT8 button, BOOL pressed, LPARAM lParam)
{
	if (pressed)
	{
		InputSystem::m_pressedKeysList[button] = TRUE;
		InputSystem::m_keyStateList[button] = TRUE;

		InputSystem::MouseInput::m_pressedKeysList[button].x = ((UINT32)(short)LOWORD(lParam));
		InputSystem::MouseInput::m_pressedKeysList[button].y = ((UINT32)(short)HIWORD(lParam));
	}
	else
	{
		InputSystem::m_releasedKeysList[button] = TRUE;
		InputSystem::m_keyStateList[button] = FALSE;

		InputSystem::MouseInput::m_releasedKeysList[button].x = ((UINT32)(short)LOWORD(lParam));
		InputSystem::MouseInput::m_releasedKeysList[button].y = ((UINT32)(short)HIWORD(lParam));
	}

	return;
}