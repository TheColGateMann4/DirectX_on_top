#pragma once

class RenderSteps;
class Shape;
class GFX;

class RenderJob
{
public:
	RenderJob(const Shape* shape, const RenderSteps* technique);

public:
	void Bind(GFX& gfx) const;

private:
	const Shape* m_shape = nullptr;
	const RenderSteps* m_step = nullptr;
};

