#include "FPSCounter.h"
#include "Includes.h"
#include "Graphics.h"
#include "imgui/imgui.h"

FPSCounter::FPSCounter(GFX& gfx)
	:
	m_screenWidth(static_cast<float>(gfx.GetWidth())),
	m_screenHeight(static_cast<float>(gfx.GetHeight()))
{

}

void FPSCounter::Draw(const float deltaTime)
{
	m_accumulatedDeltatime += deltaTime;
	m_countedTimes++;

	if (m_accumulatedDeltatime >= m_fpsCounterDelay)
	{
		float tempFps = 1.0f / (m_accumulatedDeltatime / m_countedTimes);

		fpsNumber = std::round(tempFps);

		m_accumulatedDeltatime = 0.0f;
		m_countedTimes = 0;
	}

	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs;

	{
		ImGui::PushStyleColor(ImGuiCol_Border, 0);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, m_offsetFromCorner });

		if (ImGui::Begin("FpsCounter", nullptr, windowFlags))
		{
			std::string fpsText = std::to_string(fpsNumber);

			ImVec2 textWidth = ImGui::CalcTextSize(fpsText.c_str());
			float newWindowWidth = textWidth.x + m_offsetFromCorner * 10;

			ImGui::SetWindowSize({ newWindowWidth, textWidth.y });
			ImGui::SetWindowPos({ m_screenWidth - newWindowWidth, 0.0f });


			if (textWidth.x < newWindowWidth)
				ImGui::SetCursorPosX(newWindowWidth - (textWidth.x + m_offsetFromCorner * 2));


			ImGui::Text(fpsText.c_str());
		}
		ImGui::End();

		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar();
	}
}