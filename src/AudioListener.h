#pragma once
#include "Includes.h"

namespace DirectX
{
	class AudioListener;
};

class AudioListener
{
public:
	AudioListener();

	~AudioListener();

public:
	void Update(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, float deltatime) noexcept;

	DirectX::AudioListener* Get() const noexcept;

private:
	std::unique_ptr<DirectX::AudioListener> m_audioListener;
};

