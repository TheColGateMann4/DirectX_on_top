#pragma once
#include "Includes.h"

class Camera;

class CameraManager
{
public:
	Camera* GetActiveCamera() const;

public:
	void AddCamera(Camera* camera, bool asActive);

	void SetActiveCameraByPtr(Camera* newActiveCamera);

	void SetActiveCameraByIndex(size_t newActiveCamera);

private:
	std::vector<Camera*> m_cameras = {};
	Camera* m_activeCamera = nullptr;
};

