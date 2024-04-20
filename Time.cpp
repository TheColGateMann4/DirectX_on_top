#include "Time.h"

Time::Time()
{
	m_lastTime = std::chrono::steady_clock::now();
	m_firstTime = std::chrono::steady_clock::now();
}

void Time::SetTime()
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	m_lastTime = now;
}

FLOAT Time::GetDeltaTime(bool reset)
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	const std::chrono::duration<FLOAT> time = now - m_lastTime;

	if(reset)
		m_lastTime = now;

	return time.count();
}