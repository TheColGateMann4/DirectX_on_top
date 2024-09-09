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
	virtual ~RenderStep() = default;

public:
	virtual void Render(const Shape* shape) const noexcept;

	void LinkToPipeline(class RenderGraph& renderGraph);

	void Bind(GFX& gfx) const noexcept;

	void PostRenderBind(GFX& gfx) const noexcept;

	void AddBindable(std::shared_ptr<Bindable> bindable);

	void AddPostRenderBindable(std::shared_ptr<Bindable> bindable);

public:
	template<class T>
	T* GetBindable(size_t slotNumber, TargetShader targetShader = TargetPixelShader) const noexcept
	{
		for (auto& bindable : m_bindables)
		{
			if (T* bindableOfDesiredType = dynamic_cast<T*>(bindable.get()))
				if (std::is_base_of_v<ConstantBuffer, T>)
				{
					std::vector<TargetShaderBufferBinding> targetShadersAndSlots = bindableOfDesiredType->GetShaders();

					for(const auto& target : targetShadersAndSlots)
						if ((target.slot == slotNumber) && (target.type == targetShader))
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

	template<class T>
	void RemoveBindable(std::string bindableUID)
	{
		for (size_t i = 0; i < m_bindables.size(); i++)
			if (T* bindableOfDesiredType = dynamic_cast<T*>(m_bindables.at(i).get()))
			{
				//if (bindableUID == bindableOfDesiredType.GetLocalUID())
				//{
					m_bindables.erase(m_bindables.begin() + i); // keep in mind about removing it from bindable list

					i--;
				//}
			}
	}

	RenderJobPass* GetRenderPass() const
	{
		return m_pass;
	}

protected:
	RenderJobPass* m_pass = nullptr;
	std::vector<std::shared_ptr<Bindable>> m_bindables = {};
	std::vector<std::shared_ptr<Bindable>> m_postRenderBindables = {};
	const char* m_targetPassName;
	bool m_active = true;
	bool m_linked = false;
};

class TempRenderStep : public RenderStep
{
public:
	TempRenderStep(const char* targetPassName)
		: RenderStep(targetPassName)
	{
	}

public:
	virtual void Render(const Shape* shape) const noexcept override;
};