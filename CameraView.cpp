#include "CameraView.h"
#include "imgui/imgui.h"

VOID CameraView::SetProjection(DirectX::XMMATRIX projection)
{
	this->m_projection = projection;
}

DirectX::XMMATRIX CameraView::GetCamera()
{
	const auto position = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f)
	);
	const auto projection = DirectX::XMMatrixLookAtLH(
		position, DirectX::XMVectorZero(),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	);

	return projection * DirectX::XMMatrixRotationRollPitchYaw(
		pitch, -yaw, roll
	);
}

DirectX::XMMATRIX CameraView::GetProjection()
{
	return this->m_projection;
}

VOID CameraView::Reset()
{
	r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;
	chi = 0.0f;
	roll = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
}
VOID CameraView::CreateControlMenu()
{
	if (ImGui::Begin("Camera Control"))
	{
		ImGui::Text("Positione");
		ImGui::SliderFloat("R", &r, 0.1f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -89.0, 89.0f);

		ImGui::Text("Orientatione");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);

		if (ImGui::Button("Reset"))
		{
			this->Reset();
		}

// 		ImGui::Text("Select Model");
// 
// 		if (ImGui::Button(models[curritem]))
// 			toggle = !toggle;
// 
// 		if(toggle)
// 			ImGui::ListBox(" ", &this->curritem, models, 4, 4);
	}
	ImGui::End();
}