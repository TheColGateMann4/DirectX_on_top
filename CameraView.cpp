#include "CameraView.h" 
#include "imgui/imgui.h"
#include <algorithm>
#include <random>
#include <DirectXMath.h>


CameraView::CameraView()
	:
		currentFilter(filterOptions.at(0))
{}

void CameraView::SetProjection(DirectX::XMMATRIX projection)
{
	m_projection = projection;
}

DirectX::XMMATRIX CameraView::GetCamera()
{
	const DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR upwardsVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	const DirectX::XMVECTOR lookVector = DirectX::XMVector3Transform(forwardVector,
		DirectX::XMMatrixRotationRollPitchYaw(m_view.y, m_view.x, 0.0f)
	);

	const DirectX::XMVECTOR cameraPosition = DirectX::XMLoadFloat3(&m_position);
	const DirectX::XMVECTOR lookPosition = DirectX::XMVectorAdd(cameraPosition, lookVector);
	return DirectX::XMMatrixLookAtLH(cameraPosition, lookPosition, upwardsVector);
}

DirectX::XMMATRIX CameraView::GetProjection()
{
	return m_projection;
}

void CameraView::Move(DirectX::XMFLOAT3 moveoffset)
{
	DirectX::XMFLOAT3 movedOffsetRelativeToRotation = {};

	DirectX::XMStoreFloat3(
		&movedOffsetRelativeToRotation,
		DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&moveoffset),
			DirectX::XMMatrixRotationRollPitchYaw(m_view.y, m_view.x, m_view.z) *
			DirectX::XMMatrixScaling(m_movespeed, m_movespeed, m_movespeed)
			)
		);

	m_position.x += movedOffsetRelativeToRotation.x;
	m_position.y += movedOffsetRelativeToRotation.y;
	m_position.z += movedOffsetRelativeToRotation.z;
}

void CameraView::Look(DirectX::XMFLOAT3 lookoffset)
{
	float halfRotation = 0.995f * (_Pi / 2);

	m_view.x = WrapAngle(m_view.x + lookoffset.x * m_sensivity, _Pi);					   //pitch
	m_view.y = std::clamp(m_view.y + lookoffset.y * m_sensivity, -halfRotation, halfRotation); //yaw
	m_view.z = WrapAngle(m_view.z + lookoffset.z * m_sensivity, _Pi);					   //roll
}

void CameraView::Reset()
{
	m_position = { 0.0f, 3.0f, -12.0f };
	m_view = {};
}
std::string CameraView::SpawnControlWindow()
{
	bool changedFilter = false;

	if (ImGui::Begin("Camera Control"))
	{
		ImGui::Text("Positione");
		ImGui::SliderFloat("camera X", &m_position.x, -80.0, 80.0f, "%.1f");
		ImGui::SliderFloat("camera Y", &m_position.y, -80.0, 80.0f, "%.1f");
		ImGui::SliderFloat("camera Z", &m_position.z, -80.0, 80.0f, "%.1f");

		ImGui::Text("Orientatione");
		ImGui::SliderAngle("camera Yaw", &m_view.x, -180.0f, 180.0f, "%.1f");
		ImGui::SliderAngle("camera Pitch", &m_view.y, 0.995 * -90.0f, 0.995 * 90.0f, "%.1f");
		ImGui::SliderAngle("camera Roll", &m_view.z, -180.0f, 180.0f, "%.1f");

		if (ImGui::Button("Reset"))
		{
			this->Reset();
		}

		if (ImGui::BeginCombo("Filter", currentFilter.c_str()))
		{
			for (int i = 0; i < filterOptions.size(); i++)
			{
				bool is_selected = (currentFilter == filterOptions[i]);
				if (ImGui::Selectable(filterOptions[i].c_str(), is_selected))
				{
					changedFilter = true;
					currentFilter = filterOptions[i];
				}

				if (is_selected)	
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}
	ImGui::End();


	if (changedFilter)
		return currentFilter;

	return {};
}

float CameraView::WrapAngle(float angle, float value)
{
	if (angle < value && angle > -value)
		return angle;

	float absoluteAngle = std::abs(angle);
	return ((absoluteAngle / value) - std::floor(absoluteAngle / value)) * value + (value * ((absoluteAngle != angle) ? 1 : -1));
}