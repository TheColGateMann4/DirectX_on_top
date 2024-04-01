#pragma once
#include <vector>
#include "Bindable.h"
#include "ConstantBuffers.h"

class Shape;
class RenderJobPass;

class RenderStep
{
	friend class Shape;
	friend class RenderJob;
	friend class RenderTechnique;

public:
	RenderStep(const char* targetPassName)
		: m_targetPassName(targetPassName)
	{

	}

	RenderStep()
		: m_targetPassName("unknown")
	{

	}

	RenderStep(const RenderStep&) noexcept;

public:
	void Render(const Shape* shape) const noexcept;

	void LinkToPipeline(class RenderGraph& renderGraph);

	void Bind(GFX& gfx) const noexcept;

	void AddBindable(std::shared_ptr<Bindable> bindable);

public:
	template<class T>
	T* GetBindable(size_t slotNumber, bool isPixelShader) const noexcept
	{
		for (auto& bindable : m_bindables)
		{
			if (T* bindableOfDesiredType = dynamic_cast<T*>(bindable.get()))
				if (std::is_base_of_v<ConstantBuffer, T>)
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
		result.push_back({ m_targetPassName, {} });

		for (auto& bindable : m_bindables)
			if (T* bindableOfDesiredType = dynamic_cast<T*>(bindable.get()))
				result.back().second.push_back(bindableOfDesiredType);
	}

	RenderJobPass* GetRenderPass() const
	{
		return m_pass;
	}

private:
	RenderJobPass* m_pass = nullptr;
	std::vector<std::shared_ptr<Bindable>> m_bindables = {};
	const char* m_targetPassName;
	bool m_active = true;
	bool m_linked = false;
};

