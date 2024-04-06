#pragma once
#include "Includes.h"
#include "RenderBindablePass.h"
#include "RenderJob.h"

class RenderJobPass : public RenderBindablePass
{
public:
	RenderJobPass(const char* name)
		:
		RenderBindablePass(name),
		m_jobs(),
		m_captureFrame(false)
	{}

public:
	virtual void Render(GFX& gfx) const noexcept(!_DEBUG) override;

public:
	void AddRenderJob(const RenderJob& job);

	virtual void Reset() override;

	void CaptureNextFrame();

private:
	std::vector<RenderJob> m_jobs;

	mutable bool m_captureFrame;
};

