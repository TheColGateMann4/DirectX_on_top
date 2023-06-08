#pragma once
#include "Includes.h"
#include <chrono>

class Time
{
public:
	Time();
	FLOAT Get();
	FLOAT Mark();

private:
	std::chrono::steady_clock::time_point m_lastTime;
	std::chrono::steady_clock::time_point m_firstTime;
};

