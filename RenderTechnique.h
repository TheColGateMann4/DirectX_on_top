#pragma once
#include <vector>
#include "Graphics.h"
#include "RenderSteps.h"

class RenderQueue;

class RenderTechnique
{
	friend class Shape;

public:
	void Execute(RenderQueue& renderQueue, const Shape* shape) const noexcept;

public:
	template<class T>
	T* GetBindable(size_t stepNumber = 0, size_t slotNumber = 0) const noexcept
	{
		return m_steps.at(stepNumber).GetBindable<T>(slotNumber);
	}

public:
	void AddRenderStep(const RenderSteps& renderStep)
	{
		m_steps.push_back(renderStep);
	}

private:
	std::vector<RenderSteps> m_steps = {};
};

