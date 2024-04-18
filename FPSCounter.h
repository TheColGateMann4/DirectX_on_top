#pragma once
class FPSCounter
{
public:
	FPSCounter(class GFX& gfx);

public:
	void Draw(const float deltaTime);

private:
	int fpsNumber;
	float timeCounter;
	float m_screenWidth;
	float m_screenHeight;
};

