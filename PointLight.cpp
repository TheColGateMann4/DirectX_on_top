#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(GFX& gfx, float radius)
	: m_model(gfx, radius), m_pcbuffer(gfx, 0)
{
	Reset(); // lazy setting values on startup
}

void PointLight::SpawnControlWindow(GFX& gfx) noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_pcstruct.position.x, -60.0f, 60.0f);
		ImGui::SliderFloat("Y", &m_pcstruct.position.y, -60.0f, 60.0f);
		ImGui::SliderFloat("Z", &m_pcstruct.position.z, -60.0f, 60.0f);

		ImGui::Text("Color");
		ImGui::ColorEdit3("Light Color", &m_pcstruct.lightColor.x, ImGuiColorEditFlags_NoAlpha);
		ImGui::SliderFloat("Diffuse Intensity", &m_pcstruct.diffuseIntensity, 0.00001f, 1.0f);
		ImGui::ColorEdit3("Ambient", &m_pcstruct.ambient.x, ImGuiColorEditFlags_NoAlpha);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Attenuation Const", &m_pcstruct.attenuationConst, 0.00001f, 1.0f);
		ImGui::SliderFloat("Attenuation Linear", &m_pcstruct.attenuationLinear, 0.00001f, 1.0f);
		ImGui::SliderFloat("Attenuation Quadratic", &m_pcstruct.attenuationQuadratic, 0.00001f, 1.0f);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}

		DirectX::XMFLOAT3 modelColor = m_model.GetColor();
		if (m_pcstruct.lightColor.x != modelColor.x ||
			m_pcstruct.lightColor.y != modelColor.y ||
			m_pcstruct.lightColor.z != modelColor.z )
		{
			m_model.UpdateLightColorBuffer(gfx, m_pcstruct.lightColor);
		}

	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	m_pcstruct.position = { 0.0f,0.0f,-8.0f };
	m_pcstruct.ambient = { 0.05f, 0.05f, 0.05f };
	m_pcstruct.lightColor = { 1.0f, 1.0f, 1.0f };

	m_pcstruct.diffuseIntensity = 1.0f;
	m_pcstruct.attenuationConst = 1.0f;
	m_pcstruct.attenuationLinear = 0.045f;
	m_pcstruct.attenuationQuadratic = 0.0075f;

}

void PointLight::Draw(GFX& gfx) const noexcept(!IS_DEBUG)
{
	m_model.SetPosition(m_pcstruct.position);
	m_model.Draw(gfx, 0.0f);
}

void PointLight::Bind(GFX& gfx, DirectX::XMMATRIX CameraView_) const noexcept
{
	auto temp = m_pcstruct;
	const auto position = DirectX::XMLoadFloat3(&m_pcstruct.position);
	DirectX::XMStoreFloat3(&temp.position, DirectX::XMVector3Transform(position, CameraView_));
	m_pcbuffer.Update(gfx, temp);
	m_pcbuffer.Bind(gfx);
}