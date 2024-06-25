#include "CameraManager.h"
#include "ErrorMacros.h"
#include "Graphics.h"
#include "Camera.h"

Camera* CameraManager::GetActiveCamera() const
{
	return m_activeCamera;
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