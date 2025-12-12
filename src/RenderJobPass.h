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
	virtual void Render(GFX& gfx) const noexcept(!IS_DEBUG) override;

public:
	void AddRenderJob(const RenderJob& job);

	void AddTempRenderJob(const RenderJob& tempJob);

	void Reset();

	void ResetTempModels();

	void CaptureNextFrame();

private:
	std::vector<RenderJob> m_jobs;
	std::vector<RenderJob> m_tempJobs; // jobs that should be done only once per frame, since they are camera specific

protected:
	mutable bool m_captureFrame;
};

