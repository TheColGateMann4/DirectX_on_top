#pragma once
#include "Includes.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "DepthStencilView.h"
#include "Bindable/Bindable.h"
#include "Bindable/ConstantBuffers.h"

class RenderBindablePass : public RenderPass
{
public:
	RenderBindablePass(const char* name);

public:
	virtual void CheckPassIntegrity() const override;

	template<class T>
	void AddBindableInput(const char* inputName)
	{
		static_assert(std::is_base_of_v<Bindable, T>);

		m_bindables.push_back(std::shared_ptr<T>());
		RegisterInput(RenderPassEmptyBindableInput::GetUnique(inputName, &m_bindables, m_bindables.size() - 1));
	}

	void AddBindable(std::shared_ptr<Bindable> bindable) noexcept;

	virtual void Render(GFX& gfx) const noexcept(!IS_DEBUG) override;

private:
	void Bind(GFX& gfx) const noexcept(!IS_DEBUG);


private:
	std::vector<std::shared_ptr<Bindable>> m_bindables;

protected:
	mutable bool m_bindsGraphicBuffersByItself = false;
	std::shared_ptr<RenderTarget> m_renderTarget;
	mutable std::shared_ptr<DepthStencilView> m_depthStencilView;
};

