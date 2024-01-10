#pragma once
#include <vector>
#include "Bindable.h"
#include "ConstantBuffers.h"
#include "RenderPass.h"

class RenderSteps
{
	friend class Shape;
	friend class RenderJob;
	friend class RenderTechnique;

public:
	RenderSteps(PASS_TYPE stepType, std::string name)
		: m_stepType(stepType), m_name(name)
	{}

	RenderSteps(PASS_TYPE stepType)
		: m_stepType(stepType), m_name("unknown")
	{}

public:
	void Execute(class RenderQueue& renderQueue, const Shape* shape) const noexcept;

	void Bind(GFX& gfx) const noexcept;

	void AddBindable(std::shared_ptr<Bindable> bindable);

public:
	template<class T>
	T* GetBindable(size_t slotNumber, bool isPixelShader) const noexcept
	{
		for (auto& bindable : m_bindables)
		{
			if (T* bindableOfDesiredType = dynamic_cast<T*>(bindable.get()))
				if (std::is_base_of_v<ConstantBuffer, T> || typeid(T) == typeid(ConstantBuffer))
				{
					if ((bindableOfDesiredType->GetSlot() == slotNumber) && (bindableOfDesiredType->isPixelShaderType() == isPixelShader))
						return bindableOfDesiredType;
				}
				else
					return bindableOfDesiredType;
		}

		assert(false && "Bindable could not be found in RenderStep class");
		return nullptr;
	}

	template<class T>
	void GetEveryBindableOfType(std::vector< std::pair<std::string, std::vector<T*>> >& result)
	{
		result.push_back({ m_name, {} });

		for (auto& bindable : m_bindables)
			if (T* bindableOfDesiredType = dynamic_cast<T*>(bindable.get()))
				result.back().second.push_back(bindableOfDesiredType);
	}

	PASS_TYPE GetType() const
	{
		return m_stepType;
	}

private:
	PASS_TYPE m_stepType;
	std::vector<std::shared_ptr<Bindable>> m_bindables = {};
	std::string m_name;
	bool m_active = true;
};

