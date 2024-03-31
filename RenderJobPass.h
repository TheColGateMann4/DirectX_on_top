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
		m_jobs()
	{}

public:
	virtual void Render(GFX& gfx) const noexcept(!_DEBUG) override;

public:
	void AddRenderJob(const RenderJob& job);

	virtual void Reset() override;

private:
	std::vector<RenderJob> m_jobs;
};

