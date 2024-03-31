#pragma once

class RenderStep;
class Shape;
class GFX;

class RenderJob
{
public:
	RenderJob(const Shape* shape = nullptr, const RenderStep* step = nullptr);

public:
	void Bind(GFX& gfx) const;

private:
	const Shape* m_shape;
	const RenderStep* m_step;
};

