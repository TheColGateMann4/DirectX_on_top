#pragma once
class FPSCounter
{
public:
	FPSCounter(class GFX& gfx);

public:
	void Draw(const float deltaTime);

private:
	int fpsNumber = 0;
	float m_screenWidth;
	float m_screenHeight;

	int m_countedTimes = 0;
	float m_accumulatedDeltatime = 0.0f;
};

