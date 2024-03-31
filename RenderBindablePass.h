#pragma once
#include "Includes.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "DepthStencilView.h"
#include "Bindable.h"
#include "ConstantBuffers.h"

class RenderBindablePass : public RenderPass
{
public:
	RenderBindablePass(const char* name);

public:
	virtual void CheckPassIntegrity() const override;

	template<class T>
	void AddBindableOutput(const char* outputName)
	{
		static_assert(std::is_base_of_v<Bindable, T> || typeid(Bindable) == typeid(T));

		m_bindables.push_back(std::shared_ptr<T>());
		RegisterOutput(RenderPassEmptyBindableOutput::GetUnique(outputName, &m_bindables, m_bindables.size() - 1));
	}

	void AddBindable(std::shared_ptr<Bindable> bindable) noexcept;

	virtual void Render(GFX& gfx) const noexcept(!_DEBUG) override;

private:
	void Bind(GFX& gfx) const noexcept(!_DEBUG);


private:
	std::vector<std::shared_ptr<Bindable>> m_bindables;

protected:
	std::shared_ptr<RenderTarget> m_renderTarget;
	std::shared_ptr<DepthStencilView> m_depthStencilView;
};

