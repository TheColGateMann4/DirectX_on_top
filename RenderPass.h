#pragma once
#include <vector>
#include "RenderJob.h"

enum PASS_TYPE
{
	PASS_NORMAL,
	PASS_WRITE,
	PASS_MASK
};

class RenderPass
{
public:
	void Execute(GFX& gfx)
	{
		for (const auto& job : m_jobs)
			job.Bind(gfx);
		m_jobs.clear();
	}

public:
	void AddRenderJob(const RenderJob& job)
	{
		m_jobs.push_back(job);
	}

private:
	std::vector<RenderJob> m_jobs = {};
};

