#include "AudioListener.h"
#include <Audio.h>

AudioListener::AudioListener()
	:
	m_audioListener(std::make_unique<DirectX::AudioListener>())
{

}

AudioListener::~AudioListener()
{

}

void AudioListener::Update(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, float deltatime) noexcept
{
	/*
	DirectX::XMVECTOR upvec = { 0.0f, 1.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR positionvec = { 0.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR rotationvec = { 0.0f, 0.0f, 0.0f, 0.0f };

	positionvec = DirectX::XMLoadFloat3(&position);
	rotationvec = DirectX::XMLoadFloat3(&rotation);

	m_audioListener->Update(positionvec, upvec, deltatime);
	m_audioListener->SetOrientation(rotationvec, upvec);
	*/
}

DirectX::AudioListener* AudioListener::Get() const noexcept
{
	return m_audioListener.get();
}