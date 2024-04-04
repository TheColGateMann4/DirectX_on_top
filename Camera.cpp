#include "Camera.h" 
#include <DirectXMath.h>
#include "imgui/imgui.h"
#include <algorithm>
#include <random>
#include "GaussBlurFilter.h"
#include "ErrorMacros.h"
#include "Graphics.h"

Camera::Camera(GFX& gfx)
{
	m_AspectRatio = gfx.GetWidth() / gfx.GetHeight();

	UpdateProjectionMatrix();
}

DirectX::XMMATRIX Camera::GetCameraView() const
{
	const DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR upwardsVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	const DirectX::XMVECTOR lookVector = DirectX::XMVector3Transform(forwardVector,
		DirectX::XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, m_rotation.z)
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
	float halfRotation = 0.995f * (_Pi / 2);

	m_rotation.x = WrapAngle(m_rotation.x + lookoffset.x * m_sensivity, _Pi);							//pitch
	m_rotation.y = std::clamp(m_rotation.y + lookoffset.y * m_sensivity, -halfRotation, halfRotation);	//yaw
	m_rotation.z = WrapAngle(m_rotation.z + lookoffset.z * m_sensivity, _Pi);							//roll
}

void Camera::MakeTransformPropeties(GFX& gfx)
{
	if (!GetPressedState())
		return;

	ImGui::Text("Positione");
	ImGui::SliderFloat("camera X", &m_position.x, -80.0, 80.0f, "%.1f");
	ImGui::SliderFloat("camera Y", &m_position.y, -80.0, 80.0f, "%.1f");
	ImGui::SliderFloat("camera Z", &m_position.z, -80.0, 80.0f, "%.1f");

	ImGui::Text("Orientatione");
	ImGui::SliderAngle("camera Yaw", &m_rotation.x, -180.0f, 180.0f, "%.1f");
	ImGui::SliderAngle("camera Pitch", &m_rotation.y, 0.999f * -90.0f, 0.999f * 90.0f, "%.1f");
	ImGui::SliderAngle("camera Roll", &m_rotation.z, -180.0f, 180.0f, "%.1f");

	if (ImGui::Button("Reset"))
		ResetLocalTransform();
}



void Camera::LinkSceneObjectToPipeline(RenderGraph& renderGraph)
{

}

void Camera::RenderOnScene() const noexcept(!IS_DEBUG)
{

}

void Camera::UpdateProjectionMatrix()
{
	bool setWrongEntriesProjectionMatrix = (m_Fov == 0.0f || m_AspectRatio == 0.0f || m_NearZ == 0.0f || m_FarZ == 0.0f);

	THROW_INTERNAL_ERROR("Wrong entries were set for camera projection matrix", setWrongEntriesProjectionMatrix);

	m_projection = DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_AspectRatio, m_NearZ, m_FarZ);
}

float Camera::WrapAngle(float angle, float value)
{
	if (angle < value && angle > -value)
		return angle;

	float absoluteAngle = std::abs(angle);
	return ((absoluteAngle / value) - std::floor(absoluteAngle / value)) * value + (value * ((absoluteAngle != angle) ? 1 : -1));
}