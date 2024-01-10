#include "Shape.h"
#include "SceneObject.h"


class ShapeSceneObject : public Shape, public SceneObject
{
public:
	virtual void MakePropeties(GFX& gfx) override
	{
		if (!GetPressedState())
			return;

		for (auto& technique : m_techniques)
		{
			// Setting category text so we know what are we working on in inspector
			{
				bool techniqueActive = technique.GetTechniqueActive();

				ImU32 textColor = techniqueActive ? IM_COL32(0, 255, 0, 255) : IM_COL32(128, 128, 128, 255);
				ImGui::PushStyleColor(ImGuiCol_Text, textColor);

				if (ImGui::Checkbox(technique.GetName().c_str(), &techniqueActive))
					technique.SetTechniqueActive(techniqueActive);

				ImGui::PopStyleColor();
			}


			// Writing off constant buffers
			{
				std::vector< std::pair<std::string, std::vector<CachedBuffer*>> > allStepBuffers = {};
				technique.GetEveryBindableOfType<CachedBuffer>(allStepBuffers);

				for (auto& stepBufferData : allStepBuffers)
				{
					// Writing the name of step se we know what we are working on - again
					// 		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
					//		ImGui::Text(stepBufferData.first.c_str());
					//		ImGui::PopStyleColor();

					// Writing off stuff from each buffer to set by user
					for (auto& constantBuffer : stepBufferData.second)
					{
						DynamicConstantBuffer::BufferData bufferData = constantBuffer->constBufferData;
						if (bufferData.MakeImguiMenu())
							constantBuffer->Update(gfx, bufferData);
					}
				}
			}
		}
	}

	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept override
	{
		return DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z) *
			DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	}
};