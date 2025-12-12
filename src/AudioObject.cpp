#include "AudioObject.h"
#include "AudioEngine.h"
#include <Audio.h>

#include "Graphics.h"
#include "Camera.h"

AudioObject::AudioObject(AudioEngine& audioEngine, std::wstring sound, float volume)
	:
	m_sound(L"Sounds/" + sound),
	m_volume(volume),
	m_soundEffect(std::make_unique<DirectX::SoundEffect>(audioEngine.Get(), m_sound.c_str())),
	m_soundEffectInstance(m_soundEffect->CreateInstance(DirectX::SoundEffectInstance_Use3D)),
	m_emitter(std::make_unique<DirectX::AudioEmitter>())
{

	m_emitter->CurveDistanceScaler = 100.0f;
	m_emitter->DopplerScaler = 0.1f; // scaling of out velocity distortion

	m_emitter->SetOmnidirectional();
	m_emitter->EnableDefaultMultiChannel(m_soundEffect->GetFormat()->nChannels, 10.0f);

	m_emitter->pVolumeCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&X3DAudioDefault_LinearCurve);
	m_emitter->pLFECurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&X3DAudioDefault_LinearCurve);
	m_emitter->pVolumeCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&X3DAudioDefault_LinearCurve);
}

AudioObject::~AudioObject()
{

}

void AudioObject::Update(GFX& gfx, float deltatime)
{
	DirectX::XMVECTOR positionvec = {0.0f, 0.0f, 0.0f, 0.0f};
	DirectX::XMVECTOR upvec = {0.0f, 1.0f, 0.0f, 0.0f};

	positionvec = DirectX::XMLoadFloat3(&m_position);

	m_emitter->Update(positionvec, upvec, deltatime);

	m_soundEffectInstance->Apply3D(*gfx.GetActiveCamera()->GetListener().Get(), *m_emitter);

	//m_soundEffectInstance->Play(true);
}

void AudioObject::RenderThisObjectOnScene() const noexcept(!IS_DEBUG)
{

}

void AudioObject::LinkSceneObjectToPipeline(RenderGraph& renderGraph)
{

}

const char* AudioObject::GetName() const
{
	return "AudioObject";
}