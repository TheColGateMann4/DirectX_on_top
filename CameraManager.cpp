#include "CameraManager.h"
#include "ErrorMacros.h"
#include "Graphics.h"
#include "Camera.h"

#include "Input.h"
#include "KeyMacros.h"

#include "GamePad.h" // DirecX implementation from their toolkit for easy use of gamepads

Camera* CameraManager::GetActiveCamera() const
{
	return m_activeCamera;
}

void CameraManager::HandleMovement(float deltaTime, InputSystem& hardwareInput, bool cursorLocked, bool cursorShowing)
{

	// mouse input
	{
		DirectX::XMFLOAT3 movingDir = {};
		if (hardwareInput.Key.GetKeyState(KEY_W))
			movingDir.z += deltaTime;
		if (hardwareInput.Key.GetKeyState(KEY_S))
			movingDir.z -= deltaTime;
		if (hardwareInput.Key.GetKeyState(KEY_E))
			movingDir.x += deltaTime;
		if (hardwareInput.Key.GetKeyState(KEY_A))
			movingDir.x -= deltaTime;
		if (hardwareInput.Key.GetKeyState(VK_SPACE))
			movingDir.y += deltaTime;
		if (hardwareInput.Key.GetKeyState(VK_CONTROL))
			movingDir.y -= deltaTime;

		if (movingDir.x != 0 || movingDir.y != 0 || movingDir.z != 0)
			GetActiveCamera()->Move(movingDir);

		DirectX::XMINT2 lookOffset = hardwareInput.Mouse.GetRawInputPos();

		if (cursorLocked && !cursorShowing)
			if (lookOffset.x != 0 || lookOffset.y != 0)
				GetActiveCamera()->Look({ (float)lookOffset.x, (float)lookOffset.y, 0.0f }, true);
	}

	//gamepad input
	{
		std::unique_ptr<DirectX::GamePad> gamepad = std::make_unique<DirectX::GamePad>();

		DirectX::GamePad::State gamepadState = gamepad->GetState(0, DirectX::GamePad::DEAD_ZONE_CIRCULAR); // DEAD_ZONE_CIRCULAR makes everything so much smoother, because then deadzone works as circle not as a square

		if (gamepadState.IsConnected())
		{
			DirectX::GamePad::ThumbSticks gamepadThumbSticks = gamepadState.thumbSticks;
			DirectX::GamePad::Buttons gamepadButtons = gamepadState.buttons;

			// movement
			{
				DirectX::XMFLOAT3 movingDir = {};

				movingDir.x = gamepadThumbSticks.leftX * deltaTime;
				movingDir.y = ((gamepadButtons.a ? 1.0f : 0.0f) + (gamepadButtons.b ? -1.0f : 0.0f)) * deltaTime;
				movingDir.z = gamepadThumbSticks.leftY * deltaTime;

				GetActiveCamera()->Move(movingDir);
			}

			// looking
			{
				DirectX::XMFLOAT3 lookOffset = { gamepadThumbSticks.rightX * 10.0f, -gamepadThumbSticks.rightY * 10.0f, 0.0f};

				GetActiveCamera()->Look(lookOffset, true);
			}
		}
	}
}

void CameraManager::AddCamera(Camera* camera, bool asActive)
{
	m_cameras.push_back(camera);

	if (asActive)
	{
		camera->SetActive(true);
		m_activeCamera = camera;
	}
}

void CameraManager::SetActiveCameraByPtr(Camera* newActiveCamera)
{
	if (m_activeCamera != nullptr && newActiveCamera != m_activeCamera)
		m_activeCamera->SetActive(false);

	m_activeCamera = newActiveCamera;
	m_activeCamera->SetActive(true);
}

void CameraManager::SetActiveCameraByIndex(size_t newActiveCamera)
{
	m_activeCamera = m_cameras.at(newActiveCamera);
}

void CameraManager::SetSelectedCamera(Camera* selectedCamera)
{
	m_selectedCamera = selectedCamera;
}

Camera* CameraManager::GetSelectedCamera()
{
	return m_selectedCamera;
}