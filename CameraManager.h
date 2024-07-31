#pragma once
#include "Includes.h"

class Camera;

class CameraManager
{
public:
	Camera* GetActiveCamera() const;

public:
	void HandleMovement(float deltaTime, class InputSystem& hardwareInput, bool cursorLocked, bool cursorShowing);

	void AddCamera(Camera* camera, bool asActive);

	void SetActiveCameraByPtr(Camera* newActiveCamera);

	void SetActiveCameraByIndex(size_t newActiveCamera);

	void SetSelectedCamera(Camera* selectedCamera);

	Camera* GetSelectedCamera();

private:
	std::vector<Camera*> m_cameras = {};
	Camera* m_activeCamera = nullptr;
	Camera* m_selectedCamera = nullptr;
};

