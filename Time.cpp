#include "Time.h"

Time::Time()
{
	m_lastTime = std::chrono::steady_clock::now();
	m_firstTime = std::chrono::steady_clock::now();
}

FLOAT Time::Mark()
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	const std::chrono::duration<FLOAT> time = now - m_lastTime;

	m_lastTime = now;

	return time.count();
}

FLOAT Time::Get()
{
	return static_cast<FLOAT>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_firstTime).count());
}