#pragma once
#include <vector>
#include "Graphics.h"
#include "RenderStep.h"

class RenderTechnique
{
	friend class Shape;

public:
	RenderTechnique(const char* name, bool active = true)
		:
		m_name(name),
		m_active(active)
	{}

	RenderTechnique(bool active = true)
		:
		m_name("unknown"),
		m_active(active)
	{}

public:
	void Render(const Shape* shape) const noexcept;
	void LinkToPipeline(class RenderGraph& renderGraph);

public:
	template<class T>
	T* GetBindable(size_t stepNumber, size_t slotNumber, TargetShader targetShader = TargetPixelShader) const noexcept
	{
		return m_steps.at(stepNumber)->GetBindable<T>(slotNumber, targetShader);
	}

	template<class T>
	void GetEveryBindableOfType(std::vector< std::pair<std::string, std::vector<T*>> >& result)
	{
		for (auto& step : m_steps)
			step->GetEveryBindableOfType<T>(result);
	}

public:
	template<class T,
		std::enable_if_t<std::is_base_of_v<RenderStep, T>,
			int> = 0>
	void AddRenderStep(T& renderStep)
	{
		renderStep.m_active = m_active;
		m_steps.push_back(std::make_shared<T>(renderStep));
	}

	RenderStep* GetStep(size_t stepNum)
	{
		return m_steps.at(stepNum).get();
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
			step->m_active = active;
	}

	bool GetTechniqueActive() const
	{
		return m_active;
	}

private:
	std::vector<std::shared_ptr<RenderStep>> m_steps = {};
	const char* m_name;
	bool m_active = true;
};

