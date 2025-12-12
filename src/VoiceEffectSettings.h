#pragma once
#include "Includes.h"
#include "ErrorMacros.h"
#include <xaudio2fx.h>
#include <imgui.h>

class VoiceEffectSettings
{
public:
	VoiceEffectSettings(bool is7point1 = false);

private:
	class ImguiVoiceSettingStep
	{
	public:
		virtual void Process() const
		{
			THROW_INTERNAL_ERROR("cannot process invalid imgui voice setting step");
		}
	};

	class ImguiFLOATVoiceSettingStep : public ImguiVoiceSettingStep
	{
	public:
		ImguiFLOATVoiceSettingStep(const char* name, float* pValue, float defaultValue, float minValue, float maxValue, bool* changed)
			:
			m_name(name),
			m_pValue(pValue),
			m_defaultValue(defaultValue),
			m_minValue(minValue),
			m_maxValue(maxValue),
			m_changed(changed)
		{

		}

		virtual void Process() const override
		{
			if (ImGui::SliderFloat(m_name, m_pValue, m_minValue, m_maxValue))
				*m_changed = true;
		}

	private:
		const char* m_name;
		float* m_pValue;
		float m_defaultValue;
		float m_minValue;
		float m_maxValue;
		bool* m_changed;
	};

	class ImguiUINTVoiceSettingStep : public ImguiVoiceSettingStep
	{
	public:
		ImguiUINTVoiceSettingStep(const char* name, UINT32* pValue, UINT32 defaultValue, UINT32 minValue, UINT32 maxValue, bool* changed)
			:
			m_name(name),
			m_pValue(pValue),
			m_defaultValue(defaultValue),
			m_minValue(minValue),
			m_maxValue(maxValue),
			m_changed(changed)
		{

		}

		virtual void Process() const override
		{
			if(ImGui::SliderScalar(m_name, ImGuiDataType_U32, m_pValue, &m_minValue, &m_maxValue))
				*m_changed = true;
		}

	private:
		const char* m_name;
		UINT32* m_pValue;
		UINT32 m_defaultValue;
		UINT32 m_minValue;
		UINT32 m_maxValue;
		bool* m_changed;
	};

	class ImguiBYTEVoiceSettingStep : public ImguiVoiceSettingStep
	{
	public:
		ImguiBYTEVoiceSettingStep(const char* name, BYTE* pValue, BYTE defaultValue, BYTE minValue, BYTE maxValue, bool* changed)
			:
			m_name(name),
			m_pValue(pValue),
			m_defaultValue(defaultValue),
			m_minValue(minValue),
			m_maxValue(maxValue),
			m_changed(changed)
		{

		}

		virtual void Process() const override
		{
			if(ImGui::SliderScalar(m_name, ImGuiDataType_U8, m_pValue, &m_minValue, &m_maxValue))
				*m_changed = true;
		}

	private:
		const char* m_name;
		BYTE* m_pValue;
		BYTE m_defaultValue;
		BYTE m_minValue;
		BYTE m_maxValue;
		bool* m_changed;
	};

	class ImguiBOOLVoiceSettingStep : public ImguiVoiceSettingStep
	{
	public:
		ImguiBOOLVoiceSettingStep(const char* name, BOOL* pValue, BOOL defaultValue, bool* changed)
			:
			m_name(name),
			m_pValue(pValue),
			m_defaultValue(defaultValue),
			m_changed(changed)
		{

		}

		virtual void Process() const override
		{
			if (ImGui::Checkbox(m_name, reinterpret_cast<bool*>(m_pValue)))
				*m_changed = true;
		}

	private:
		const char* m_name;
		BOOL* m_pValue;
		BOOL m_defaultValue;
		bool* m_changed;
	};

private:
	void HandleVoiceSetting(const char* name, float* pValue, float defaultValue, float minValue, float maxValue)
	{
		*pValue = defaultValue;

		imguiSteps.push_back(std::make_unique<ImguiFLOATVoiceSettingStep>(name, pValue, defaultValue, minValue, maxValue, &m_changed));
	}

	void HandleVoiceSetting(const char* name, BYTE* pValue, BYTE defaultValue, BYTE minValue, BYTE maxValue)
	{
		*pValue = defaultValue;

		imguiSteps.push_back(std::make_unique<ImguiBYTEVoiceSettingStep>(name, pValue, defaultValue, minValue, maxValue, &m_changed));
	}

	void HandleVoiceSetting(const char* name, UINT32* pValue, UINT32 defaultValue, UINT32 minValue, UINT32 maxValue)
	{
		*pValue = defaultValue;

		imguiSteps.push_back(std::make_unique<ImguiUINTVoiceSettingStep>(name, pValue, defaultValue, minValue, maxValue, &m_changed));
	}

	void HandleVoiceSetting(const char* name, BOOL* pValue, BOOL defaultValue)
	{
		*pValue = defaultValue;

		imguiSteps.push_back(std::make_unique<ImguiBOOLVoiceSettingStep>(name, pValue, defaultValue, &m_changed));
	}

public:
	void DrawEffectSettings();

	const XAUDIO2FX_REVERB_PARAMETERS* Get() const;

	bool SettingsChanged() const;

private:
	std::vector<std::unique_ptr<ImguiVoiceSettingStep>> imguiSteps;
	XAUDIO2FX_REVERB_PARAMETERS m_settingData = {};	// the structure only contains UINT32, BYTE, float and bool. These are the types we need to handle
	
	bool m_changed = false;
};

/*

typedef struct XAUDIO2FX_REVERB_PARAMETERS
{
	// ratio of wet (processed) signal to dry (original) signal
	float WetDryMix;            // [0, 100] (percentage)

	// Delay times
	UINT32 ReflectionsDelay;    // [0, 300] in ms
	BYTE ReverbDelay;           // [0, 85] in ms
	BYTE RearDelay;             // 7.1: [0, 20] in ms, all other: [0, 5] in ms
#if(_WIN32_WINNT >= _WIN32_WINNT_WIN10)
	BYTE SideDelay;             // 7.1: [0, 5] in ms, all other: not used, but still validated
#endif


	// Indexed parameters
	BYTE PositionLeft;          // [0, 30] no units
	BYTE PositionRight;         // [0, 30] no units, ignored when configured to mono
	BYTE PositionMatrixLeft;    // [0, 30] no units
	BYTE PositionMatrixRight;   // [0, 30] no units, ignored when configured to mono
	BYTE EarlyDiffusion;        // [0, 15] no units
	BYTE LateDiffusion;         // [0, 15] no units
	BYTE LowEQGain;             // [0, 12] no units
	BYTE LowEQCutoff;           // [0, 9] no units
	BYTE HighEQGain;            // [0, 8] no units
	BYTE HighEQCutoff;          // [0, 14] no units

	// Direct parameters
	float RoomFilterFreq;       // [20, 20000] in Hz
	float RoomFilterMain;       // [-100, 0] in dB
	float RoomFilterHF;         // [-100, 0] in dB
	float ReflectionsGain;      // [-100, 20] in dB
	float ReverbGain;           // [-100, 20] in dB
	float DecayTime;            // [0.1, inf] in seconds
	float Density;              // [0, 100] (percentage)
	float RoomSize;             // [1, 100] in feet

	// component control
	BOOL DisableLateField;      // TRUE to disable late field reflections
} XAUDIO2FX_REVERB_PARAMETERS;

*/