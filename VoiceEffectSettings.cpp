#include "VoiceEffectSettings.h"

VoiceEffectSettings::VoiceEffectSettings(bool is7point1)
{
	HandleVoiceSetting("wet-dry mix",			&m_settingData.WetDryMix,			XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX,		 XAUDIO2FX_REVERB_MIN_WET_DRY_MIX,        XAUDIO2FX_REVERB_MAX_WET_DRY_MIX);
	HandleVoiceSetting("reflections delay",		&m_settingData.ReflectionsDelay,	XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY,  XAUDIO2FX_REVERB_MIN_REFLECTIONS_DELAY,  XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY);
	HandleVoiceSetting("reverb delay",			&m_settingData.ReverbDelay,			XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY,		 XAUDIO2FX_REVERB_MIN_REVERB_DELAY,		  XAUDIO2FX_REVERB_MAX_REVERB_DELAY);
	HandleVoiceSetting("rear delay",			&m_settingData.RearDelay,			XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY,		 XAUDIO2FX_REVERB_MIN_REAR_DELAY,		  XAUDIO2FX_REVERB_MAX_REAR_DELAY);
	HandleVoiceSetting("side delay",			&m_settingData.SideDelay,			XAUDIO2FX_REVERB_DEFAULT_7POINT1_SIDE_DELAY, XAUDIO2FX_REVERB_MIN_7POINT1_SIDE_DELAY, XAUDIO2FX_REVERB_MAX_7POINT1_SIDE_DELAY);
	HandleVoiceSetting("position right",		&m_settingData.PositionRight,		XAUDIO2FX_REVERB_DEFAULT_POSITION,			 XAUDIO2FX_REVERB_MIN_POSITION,			  XAUDIO2FX_REVERB_MAX_POSITION);
	HandleVoiceSetting("position left",			&m_settingData.PositionLeft,		XAUDIO2FX_REVERB_DEFAULT_POSITION,			 XAUDIO2FX_REVERB_MIN_POSITION,			  XAUDIO2FX_REVERB_MAX_POSITION);
	HandleVoiceSetting("position matrix right",	&m_settingData.PositionMatrixRight, XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX,	 XAUDIO2FX_REVERB_MIN_POSITION,           XAUDIO2FX_REVERB_MAX_POSITION);
	HandleVoiceSetting("position matrix left",	&m_settingData.PositionMatrixLeft,	XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX,	 XAUDIO2FX_REVERB_MIN_POSITION,			  XAUDIO2FX_REVERB_MAX_POSITION);
	HandleVoiceSetting("early diffusion",		&m_settingData.EarlyDiffusion,		XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION,	 XAUDIO2FX_REVERB_MIN_DIFFUSION,	      XAUDIO2FX_REVERB_MAX_DIFFUSION);
	HandleVoiceSetting("late diffusion",		&m_settingData.LateDiffusion,		XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION,	 XAUDIO2FX_REVERB_MIN_DIFFUSION,		  XAUDIO2FX_REVERB_MAX_DIFFUSION);
	HandleVoiceSetting("low eq gain",			&m_settingData.LowEQGain,			XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN,		 XAUDIO2FX_REVERB_MIN_LOW_EQ_GAIN,		  XAUDIO2FX_REVERB_MAX_LOW_EQ_GAIN);
	HandleVoiceSetting("low eq cutoff",			&m_settingData.LowEQCutoff,			XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF,		 XAUDIO2FX_REVERB_MIN_LOW_EQ_CUTOFF,	  XAUDIO2FX_REVERB_MAX_LOW_EQ_CUTOFF);
	HandleVoiceSetting("high eq gain",			&m_settingData.HighEQGain,			XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN,		 XAUDIO2FX_REVERB_MIN_HIGH_EQ_GAIN,		  XAUDIO2FX_REVERB_MAX_HIGH_EQ_GAIN);
	HandleVoiceSetting("high eq cutoff",		&m_settingData.HighEQCutoff,		XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF,	 XAUDIO2FX_REVERB_MIN_HIGH_EQ_CUTOFF,	  XAUDIO2FX_REVERB_MAX_HIGH_EQ_CUTOFF);
	HandleVoiceSetting("room filter freq",		&m_settingData.RoomFilterFreq,		XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ,	 XAUDIO2FX_REVERB_MIN_ROOM_FILTER_FREQ,	  XAUDIO2FX_REVERB_MAX_ROOM_FILTER_FREQ);
	HandleVoiceSetting("room filter main",		&m_settingData.RoomFilterMain,		XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN,	 XAUDIO2FX_REVERB_MIN_ROOM_FILTER_MAIN,	  XAUDIO2FX_REVERB_MAX_ROOM_FILTER_MAIN);
	HandleVoiceSetting("room filter hf",		&m_settingData.RoomFilterHF,		XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF,	 XAUDIO2FX_REVERB_MIN_ROOM_FILTER_HF,	  XAUDIO2FX_REVERB_MAX_ROOM_FILTER_HF);
	HandleVoiceSetting("reflections gain",		&m_settingData.ReflectionsGain,		XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN,	 XAUDIO2FX_REVERB_MIN_REFLECTIONS_GAIN,	  XAUDIO2FX_REVERB_MAX_REFLECTIONS_GAIN);
	HandleVoiceSetting("reverb gain",			&m_settingData.ReverbGain,			XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN,		 XAUDIO2FX_REVERB_MIN_REVERB_GAIN,		  XAUDIO2FX_REVERB_MAX_REVERB_GAIN);
	HandleVoiceSetting("decay time",			&m_settingData.DecayTime,			XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME,		 XAUDIO2FX_REVERB_MIN_DECAY_TIME,		  50.0f); // max can go up to FLT_MAX / 2.0f but its no point since its weird
	HandleVoiceSetting("density",				&m_settingData.Density,				XAUDIO2FX_REVERB_DEFAULT_DENSITY,			 XAUDIO2FX_REVERB_MIN_DENSITY,			  XAUDIO2FX_REVERB_MAX_DENSITY);
	HandleVoiceSetting("room size",				&m_settingData.RoomSize,			XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE,			 XAUDIO2FX_REVERB_MIN_ROOM_SIZE,		  XAUDIO2FX_REVERB_MAX_ROOM_SIZE);
	HandleVoiceSetting("disable late field",	&m_settingData.DisableLateField,	XAUDIO2FX_REVERB_DEFAULT_DISABLE_LATE_FIELD);
}

void VoiceEffectSettings::DrawEffectSettings()
{
	m_changed = false;

	for (const auto& step : imguiSteps)
		step->Process();
}

const XAUDIO2FX_REVERB_PARAMETERS* VoiceEffectSettings::Get() const
{
	return &m_settingData;
}

bool VoiceEffectSettings::SettingsChanged() const
{
	return m_changed;
}