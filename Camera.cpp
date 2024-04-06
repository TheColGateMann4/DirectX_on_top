#include "Camera.h" 
#include <DirectXMath.h>
#include "imgui/imgui.h"
#include <algorithm>
#include <random>
#include "GaussBlurFilter.h"
#include "ErrorMacros.h"
#include "Graphics.h"
#include "CameraManager.h"

Camera::Camera(GFX& gfx, CameraManager* cameraManager)
	:
	m_AspectRatio((float)gfx.GetWidth() / (float)gfx.GetHeight()),
	m_cameraManager(cameraManager),
	m_indicator(gfx, this),
	m_viewIndicator(gfx, this)
{

	UpdateProjectionMatrix(gfx);
}

DirectX::XMMATRIX Camera::GetCameraView() const
{
	const DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR upwardsVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	if (m_rotation.z != 0.0f)
		upwardsVector = DirectX::XMVector3Rotate(upwardsVector, DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m_rotation.z));

	const DirectX::XMVECTOR lookVector = DirectX::XMVector3Transform(forwardVector,
		DirectX::XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, 0.0f)
	);

	const DirectX::XMVECTOR cameraPosition = DirectX::XMLoadFloat3(&m_position);
	const DirectX::XMVECTOR lookPosition = DirectX::XMVectorAdd(cameraPosition, lookVector);
	return DirectX::XMMatrixLookAtLH(cameraPosition, lookPosition, upwardsVector);
}

DirectX::XMMATRIX Camera::GetProjection() const
{
	return m_projection;
}

void Camera::Move(const DirectX::XMFLOAT3& moveoffset)
{
	DirectX::XMFLOAT3 movedOffsetRelativeToRotation = {};

	DirectX::XMStoreFloat3(
		&movedOffsetRelativeToRotation,
		DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&moveoffset),
			DirectX::XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, 0.0f) *
			DirectX::XMMatrixScaling(m_movespeed, m_movespeed, m_movespeed)
			)
		);


	m_position.x += movedOffsetRelativeToRotation.x;
	m_position.y += movedOffsetRelativeToRotation.y;
	m_position.z += movedOffsetRelativeToRotation.z;
}

void Camera::Look(const DirectX::XMFLOAT3 lookoffset)
{
	float halfRotation = 0.999f * (_Pi / 2);

	m_rotation.x = WrapAngle(m_rotation.x + lookoffset.x * m_sensivity, _Pi);							//pitch
	m_rotation.y = std::clamp(m_rotation.y + lookoffset.y * m_sensivity, -halfRotation, halfRotation);	//yaw
	m_rotation.z = WrapAngle(m_rotation.z + lookoffset.z * m_sensivity, _Pi);							//roll
}

void Camera::Reset(GFX& gfx)
{
	m_movespeed = 12.0f;
	m_sensivity = 0.008f;

	m_Fov = _Pi / 2;
	m_NearZ = 0.5f;
	m_FarZ = 400.0f;

	UpdateProjectionMatrix(gfx);
}

void Camera::MakeTransformPropeties(GFX& gfx)
{
	if (!GetPressedState())
	{
		m_selected = false;
		return;
	}

	m_selected = true;
	
	if (ImGui::Button("Activate"))
	{
		m_active = true;
		m_cameraManager->SetActiveCameraByPtr(this);
	}


	ImGui::Text("Positione");
	ImGui::SliderFloat("camera X", &m_position.x, -80.0, 80.0f, "%.1f");
	ImGui::SliderFloat("camera Y", &m_position.y, -80.0, 80.0f, "%.1f");
	ImGui::SliderFloat("camera Z", &m_position.z, -80.0, 80.0f, "%.1f");

	ImGui::Text("Orientatione");
	ImGui::SliderAngle("camera Yaw", &m_rotation.x, -180.0f, 180.0f, "%.1f");
	ImGui::SliderAngle("camera Pitch", &m_rotation.y, 0.999f * -90.0f, 0.999f * 90.0f, "%.1f");
	ImGui::SliderAngle("camera Roll", &m_rotation.z, -180.0f, 180.0f, "%.1f");

	if (ImGui::Button("Reset Transform"))
		ResetLocalTransform();
}

void Camera::MakePropeties(GFX& gfx)
{
	if (!GetPressedState())
		return;

	//we check if something changed to determine if we should update our projection matrix
	bool changed = false;

	auto checkChanged = [&changed](bool returnFromStatement) mutable
		{
			changed = changed || returnFromStatement;
		};

	ImGui::SliderFloat("moving speed", &m_movespeed, 1.0f, 100.0f, "%.1f");
	ImGui::SliderFloat("sensivity", &m_sensivity, 0.001f, 0.1f, "%.3f");

	checkChanged(ImGui::SliderAngle("fov", &m_Fov, 30.0f, 170.0f, "%.1f"));
	checkChanged(ImGui::SliderFloat("nearZ", &m_NearZ, 0.1f, (m_FarZ < 10.0f) ? m_FarZ - 1.0f : 10.0f, "%.1f"));
	checkChanged(ImGui::SliderFloat("farZ", &m_FarZ, m_NearZ + 1.0f, 1000.0f, "%.1f"));

	if (changed)
		UpdateProjectionMatrix(gfx);

	if (ImGui::Button("Reset Camera Settings"))
		Reset(gfx);
}

void Camera::LinkSceneObjectToPipeline(RenderGraph& renderGraph)
{
	m_indicator.LinkToPipeline(renderGraph);
	m_viewIndicator.LinkToPipeline(renderGraph);
}

void Camera::RenderOnScene() const noexcept(!IS_DEBUG)
{
	if (!m_active) 
	{
		m_indicator.Render();

		if (m_selected)
			m_viewIndicator.Render();	
	}
}

void Camera::UpdateProjectionMatrix(GFX& gfx)
{
	bool setWrongEntriesProjectionMatrix = (m_Fov == 0.0f || m_AspectRatio == 0.0f || m_NearZ == 0.0f || m_FarZ == 0.0f);

	THROW_INTERNAL_ERROR("Wrong entries were set for camera projection matrix", setWrongEntriesProjectionMatrix);

	m_projection = DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_AspectRatio, m_NearZ, m_FarZ);

	m_viewIndicator.UpdateVertexBuffer(gfx);
}

float Camera::WrapAngle(float angle, float value)
{
	if (angle < value && angle > -value)
		return angle;

	float absoluteAngle = std::abs(angle);
	return ((absoluteAngle / value) - std::floor(absoluteAngle / value)) * value + (value * ((absoluteAngle != angle) ? 1 : -1));
}