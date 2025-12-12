#pragma once
#include "Includes.h"
#include <chrono>

class Time
{
public:
	Time();
	void SetTime();
	FLOAT GetDeltaTime(bool reset = false);

private:
	std::chrono::steady_clock::time_point m_lastTime;
	std::chrono::steady_clock::time_point m_firstTime;
};

