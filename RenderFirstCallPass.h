#pragma once
#include "RenderJobPass.h"

// this is just base class, inherited classes handle the process
class RenderFirstCallPass
{
public:
	void FrameStarted() noexcept;

protected:
	virtual void FirstRenderCall(GFX& gfx) const noexcept(!IS_DEBUG) = 0;

	virtual void FurtherRenderCall(GFX& gfx) const noexcept(!IS_DEBUG) = 0;

protected:
	mutable bool m_called = false;
};

class RenderFirstCallJobPass : public RenderFirstCallPass, public RenderJobPass
{
public:
	RenderFirstCallJobPass(const char* jobPassName);

public:
	virtual void Render(GFX& gfx) const noexcept(!IS_DEBUG) override;
};