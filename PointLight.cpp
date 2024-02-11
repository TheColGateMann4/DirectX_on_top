#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(GFX& gfx, float radius)
	: m_model(gfx, radius)
{
	DynamicConstantBuffer::BufferLayout layout;

	layout.Add<DynamicConstantBuffer::DataType::Float3>("position");
	layout.Add<DynamicConstantBuffer::DataType::Float3>("ambient");
	layout.Add<DynamicConstantBuffer::DataType::Float3>("lightColor");

	layout.Add<DynamicConstantBuffer::DataType::Float>("diffuseIntensity");
	layout.Add<DynamicConstantBuffer::DataType::Float>("attenuationConst");
	layout.Add<DynamicConstantBuffer::DataType::Float>("attenuationLinear");
	layout.Add<DynamicConstantBuffer::DataType::Float>("attenuationQuadratic");

	layout.GetFinished();

	constBufferData = DynamicConstantBuffer::BufferData(layout);

	m_pcbuffer = NonCachedBuffer(gfx, layout, 0, true);

	Reset(); // lazy setting values on startup
}



void PointLight::MakeTransformPropeties(GFX& gfx)
{
	if (!GetPressedState())
		return;

	DirectX::XMFLOAT3& position = *constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("position");

	ImGui::Text("Position");
	ImGui::SliderFloat("X", &position.x, -60.0f, 60.0f);
	ImGui::SliderFloat("Y", &position.y, -60.0f, 60.0f);
	ImGui::SliderFloat("Z", &position.z, -60.0f, 60.0f);
}

void PointLight::MakeAdditionalPropeties(GFX& gfx, float deltaTime)
{
	DynamicConstantBuffer::BufferData& bufferData = constBufferData;

	DirectX::XMFLOAT3* lightColor = bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("lightColor");

	if (GetPressedState())
	{
		ImGui::Text("Color");
		ImGui::ColorEdit3("Light Color", reinterpret_cast<float*>(lightColor), ImGuiColorEditFlags_NoAlpha);

		ImGui::Checkbox("Chroma Light", &enableChroma);

		if (enableChroma)
		{
			ImGui::SliderFloat("Chroma Delta Time", &chromaDeltaTime, 0.001f, 200.0f);
		}

		ImGui::SliderFloat("Diffuse Intensity", bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("diffuseIntensity"), 0.01f, 2.0f, "%.2f");
		ImGui::ColorEdit3("Ambient", reinterpret_cast<float*>(bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("ambient")), ImGuiColorEditFlags_NoAlpha);

		ImGui::Text("Falloff");

		ImGui::SliderFloat("Attenuation Const", bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("attenuationConst"), 0.05f, 10.0f, "%.2f");
		ImGui::SliderFloat("Attenuation Linear", bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("attenuationLinear"), 0.0001f, 4.0f, "%.4f");
		ImGui::SliderFloat("Attenuation Quadratic", bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("attenuationQuadratic"), 0.00001f, 1.0f);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}

	if (enableChroma && deltaTime - lastDeltaTime >= chromaDeltaTime)
	{
		int incrementColor = colorToDecrement == 2 ? 0 : colorToDecrement + 1;
		float* pIncrementColor = &((float*)lightColor)[incrementColor];
		float* pDecrementColor = &((float*)lightColor)[colorToDecrement];


		if (!incrementingNow && *pDecrementColor <= 0.0f)
		{
			incrementingNow = true;
			justSwitched = true;
			*pDecrementColor = 0.0f;

			if (colorToDecrement < 2)
				colorToDecrement++;
			else
				colorToDecrement = 0;
		}
		else if (incrementingNow && *pIncrementColor >= 1.0f)
		{
			incrementingNow = false;
			justSwitched = true;
			*pIncrementColor = 1.0f;
		}


		if (!justSwitched)
			if (incrementingNow)
				*pIncrementColor += 0.003f;
			else
				*pDecrementColor -= 0.003f;

		justSwitched = false;
		lastDeltaTime = deltaTime;
	}

	if(GetPressedState() || enableChroma)
	{
		const DirectX::XMFLOAT3& modelColor = m_model.GetColor();

		if (lightColor->x != modelColor.x ||
			lightColor->y != modelColor.y ||
			lightColor->z != modelColor.z)
		{
			m_model.UpdateLightColorBuffer(gfx, *lightColor);
		}
	}
}

void PointLight::RenderOnScene(RenderQueue& renderQueue) const noexcept(!IS_DEBUG)
{
	m_model.SetPosition(*constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("position"));
	m_model.Render(renderQueue);
}

void PointLight::Reset() noexcept
{
	DynamicConstantBuffer::BufferData &bufferData = constBufferData;

	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("position") = { -1.4f, 3.0f, 1.35f };
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("ambient") = { 0.05f, 0.05f, 0.05f };
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("lightColor") = { 1.0f, 1.0f, 1.0f };

	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("diffuseIntensity") = 1.0f;
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("attenuationConst") = 1.0f;
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("attenuationLinear") = 0.045f;
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("attenuationQuadratic") = 0.0075f;

}

void PointLight::Bind(GFX& gfx, DirectX::XMMATRIX CameraView_) const noexcept
{
	DynamicConstantBuffer::BufferData temp = constBufferData;
	const auto position = DirectX::XMLoadFloat3(constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("position"));
	DirectX::XMStoreFloat3(temp.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("position"), DirectX::XMVector3Transform(position, CameraView_));
	m_pcbuffer.Update(gfx, temp);
	m_pcbuffer.Bind(gfx);
}