#include "AudioEngine.h"
#include <Audio.h>
#include <imgui.h>
#include "ErrorMacros.h"
#include "VoiceEffectSettings.h"

AudioEngine::AudioEngine()
	:
	m_num_channels(2),
	m_currentVolume(1.0f),
	m_audioEngine(nullptr),
	m_pActiveAudioDevice(nullptr),
	m_audioEffectSettings(nullptr)
{

}

AudioEngine::~AudioEngine()
{

}

void AudioEngine::Initialize()
{
	// initializing directX audio engine
	DirectX::AUDIO_ENGINE_FLAGS engineFlags = DirectX::AudioEngine_Default | DirectX::AudioEngine_ThrowOnNoAudioHW | DirectX::AudioEngine_ReverbUseFilters | DirectX::AudioEngine_EnvironmentalReverb;

#ifdef _DEBUG
	engineFlags |= DirectX::AudioEngine_Debug;
#endif

	m_audioEngine = std::make_unique<DirectX::AudioEngine>(engineFlags);

	// calling it first from here
	// when devices get added during runtime we will call the same function from message loop in window.cpps
	this->UpdateAudioDeviceList();


	// creating effect chain
	{
		IUnknown* pXAPO;
		HRESULT hr;

		THROW_GFX_IF_FAILED(XAudio2CreateReverb(&pXAPO));

		XAUDIO2_EFFECT_DESCRIPTOR descriptor;
		descriptor.InitialState = true;
		descriptor.OutputChannels = m_num_channels;
		descriptor.pEffect = pXAPO;

		XAUDIO2_EFFECT_CHAIN chain;
		chain.EffectCount = 1;
		chain.pEffectDescriptors = &descriptor;

		THROW_GFX_IF_FAILED(m_audioEngine->GetMasterVoice()->SetEffectChain(&chain));

		pXAPO->Release();
	}

	// voice effect settings class for easier use
	{
		m_audioEffectSettings = std::make_unique<VoiceEffectSettings>();
	}
}

void AudioEngine::ShowWindow(GFX& gfx, bool show)
{
	if (!show)
		return;

	if (ImGui::Begin("Audio Settings"))
	{
		// Audio Devices list
		{
			ImGui::Text("Audio Devices");

			if(ImGui::BeginListBox("##empty", { 400, 0 }))// 0 means size will be set to default
			{
				for (const auto& audioDevice : m_audioDevices)
					if (ImGui::Selectable(audioDevice.first.c_str(), m_pActiveAudioDevice == &audioDevice))
					{
						m_audioEngine->Reset(nullptr, audioDevice.second.c_str()); // second = deviceID
						m_pActiveAudioDevice = &audioDevice;
					}

				ImGui::EndListBox();
			}
		}

		// some spacing for better readability
		{
			ImGui::Spacing();
			ImGui::Spacing();
		}

		// Volume slider
		{
			if (ImGui::SliderFloat("Volume", &m_currentVolume, 0.0f, 1.0f, "%.1f"))
				m_audioEngine->SetMasterVolume(m_currentVolume);
		}

		// some spacing for better readability
		{
			ImGui::Spacing();
			ImGui::Spacing();
		}

		// drawing our audio effect settings
		{
			m_audioEffectSettings->DrawEffectSettings();
		}

		// debug stats button
		if(ImGui::Button("Debug Stats"))
		{
			auto stats = m_audioEngine->GetStatistics();

			std::cout << "\nPlaying: " << stats.playingOneShots << " / " << stats.playingInstances << "; Instances " << stats.allocatedInstances << "; Voices " << stats.allocatedVoices << " / " << stats.allocatedVoices3d << " / " << stats.allocatedVoicesOneShot << " / " << stats.allocatedVoicesIdle << "; " << stats.audioBytes << " audio bytes, " << stats.streamingBytes << " streaming buffer bytes %\n";
		}
	}

	ImGui::End();
}

void AudioEngine::Update()
{
	if (!m_audioEngine->Update())
	{
		// No audio device is active
		if (m_audioEngine->IsCriticalError())
		{
			THROW_INTERNAL_ERROR("no audio device is active");
		}
	}


	// updating effect settings with our structure
	if(m_audioEffectSettings->SettingsChanged())
	{
		HRESULT hr;

		const XAUDIO2FX_REVERB_PARAMETERS* audioEffectSettings = m_audioEffectSettings->Get();

		THROW_GFX_IF_FAILED(
			m_audioEngine->GetMasterVoice()->SetEffectParameters(
				0,
				audioEffectSettings,
				sizeof(*audioEffectSettings)
			)
		);
	}
}

DirectX::AudioEngine* AudioEngine::Get() const noexcept
{
	return m_audioEngine.get();
}

void AudioEngine::UpdateAudioDeviceList()
{
	m_audioDevices.clear();

	std::vector<DirectX::AudioEngine::RendererDetail> audioDevicesList = DirectX::AudioEngine::GetRendererDetails();

	for (auto audioDevice : audioDevicesList)
		m_audioDevices.push_back({ std::string(audioDevice.description.begin(), audioDevice.description.end()), audioDevice.deviceId });
}