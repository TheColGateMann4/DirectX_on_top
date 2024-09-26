#pragma once
#include "Includes.h"

namespace DirectX
{
	class AudioEngine;
};

class AudioEngine
{
public:
	AudioEngine();

	~AudioEngine();

public:
	void Initialize();

	DirectX::AudioEngine* Get() const noexcept;

	void ShowWindow(class GFX& gfx, bool show);

	void Update();

	void UpdateAudioDeviceList();

private:
	UINT8 m_num_channels;
	float m_currentVolume;
	std::unique_ptr<DirectX::AudioEngine> m_audioEngine;

	typedef std::pair<std::string, std::wstring> t_device_structure;

	const t_device_structure* m_pActiveAudioDevice;
	std::vector<t_device_structure> m_audioDevices;
	std::unique_ptr<class VoiceEffectSettings> m_audioEffectSettings;
};