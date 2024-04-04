#include "CameraManager.h"
#include "ErrorMacros.h"
#include "Graphics.h"
#include "Camera.h"

CameraManager::CameraManager(GFX& gfx)
{
	gfx.SetActiveCameraLinkage(&m_activeCamera);
}

Camera* CameraManager::GetActiveCamera() const
{
	return m_activeCamera;
}

void CameraManager::AddCamera(Camera* camera, bool asActive)
{
	m_cameras.push_back(camera);

	if (asActive)
	{
		camera->m_active = true;
		m_activeCamera = camera;
	}
}

void CameraManager::SetActiveCameraByPtr(Camera* newActiveCamera)
{
	if (m_activeCamera != nullptr)
		m_activeCamera->m_active = false;

	m_activeCamera = newActiveCamera;
}

void CameraManager::SetActiveCameraByIndex(size_t newActiveCamera)
{
	m_activeCamera = m_cameras.at(newActiveCamera);
}