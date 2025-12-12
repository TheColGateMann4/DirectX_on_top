#pragma once
#include "SceneObject.h"

namespace DirectX
{
	class SoundEffect;
	class AudioEmitter;
	class SoundEffectInstance;
};

class AudioObject : public SceneObject
{
public:
	AudioObject(class AudioEngine& audioEngine, std::wstring sound = L"Coulds-Huma-Huma.wav", float volume = 1.0f);

	~AudioObject();

public:
	virtual void Update(GFX& gfx, float deltatime) override;

	virtual void RenderThisObjectOnScene() const noexcept(!IS_DEBUG) override;

	virtual void LinkSceneObjectToPipeline(RenderGraph& renderGraph) override;

	virtual const char* GetName() const override;

private:
	std::wstring m_sound;
	float m_volume;

	std::unique_ptr<DirectX::SoundEffect> m_soundEffect; // for now only one sound
	std::unique_ptr<DirectX::SoundEffectInstance> m_soundEffectInstance;
	std::unique_ptr<DirectX::AudioEmitter> m_emitter;

	//Image objectImage;
};

