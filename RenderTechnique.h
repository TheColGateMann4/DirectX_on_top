#pragma once
#include <vector>
#include "Graphics.h"
#include "RenderSteps.h"

class RenderQueue;

class RenderTechnique
{
	friend class Shape;

public:
	RenderTechnique(std::string name)
		:
		m_name(name)
	{}

	RenderTechnique()
		:
		m_name("unknown")
	{}

public:
	void Execute(RenderQueue& renderQueue, const Shape* shape) const noexcept;

public:
	template<class T>
	T* GetBindable(size_t stepNumber, size_t slotNumber, bool isPixelShader) const noexcept
	{
		return m_steps.at(stepNumber).GetBindable<T>(slotNumber, isPixelShader);
	}

	template<class T>
	void GetEveryBindableOfType(std::vector< std::pair<std::string, std::vector<T*>> >& result)
	{
		for (auto& step : m_steps)
			step.GetEveryBindableOfType<T>(result);
	}

public:
	void AddRenderStep(const RenderSteps& renderStep)
	{
		m_steps.push_back(renderStep);
	}

public:
	std::string GetName() const noexcept
	{
		return m_name;
	}

public:
	void SetTechniqueActive(bool active)
	{
		m_active = active;

		for (auto& step : m_steps)
			step.m_active = active;
	}

	bool GetTechniqueActive()
	{
		return m_active;
	}

private:
	std::vector<RenderSteps> m_steps = {};
	std::string m_name;
	bool m_active = true;
};

