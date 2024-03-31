#include "RenderBindablePass.h"
#include "ErrorMacros.h"

RenderBindablePass::RenderBindablePass(const char* name)
	: RenderPass(name)
{

}

void RenderBindablePass::CheckPassIntegrity() const
{
	RenderPass::CheckPassIntegrity();

	if (m_depthStencilView == nullptr && m_renderTarget == nullptr)
	{
		std::string errorString = "DepthStencilView wasn't set for RenderBindablePass with name: \"";
		errorString += this->GetName();
		errorString += "\".\n";

		THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
	}
}

void RenderBindablePass::AddBindable(std::shared_ptr<Bindable> bindable) noexcept
{
	m_bindables.push_back(bindable);
}

void RenderBindablePass::Render(GFX& gfx) const noexcept(!_DEBUG)
{
	Bind(gfx);

	for (auto& bindable : m_bindables)
		bindable->Bind(gfx);
}

void RenderBindablePass::Bind(GFX& gfx) const noexcept(!_DEBUG)
{
	if(m_renderTarget != nullptr)
		m_renderTarget->BindRenderTarget(gfx, m_depthStencilView.get());
	else
		m_depthStencilView->BindRenderTarget(gfx);
}