#include "Shape.h"
#include "BindableClassesMacro.h"
#include "RenderJob.h"

void Shape::Render() const noexcept(!IS_DEBUG)
{
	for (const auto& technique : m_techniques)
		technique.Render(this);
}

void Shape::LinkToPipeline(class RenderGraph& renderGraph)
{
	for (auto& technique : m_techniques)
		technique.LinkToPipeline(renderGraph);
}

void Shape::Bind(GFX& gfx) const noexcept
{
	m_pIndexBuffer->Bind(gfx);

	m_pTransformConstBuffer->Bind(gfx);

	m_pVertexBuffer->Bind(gfx);

	if(m_pTopology)
		m_pTopology->Bind(gfx);
}

void Shape::AddRenderTechnique(const RenderTechnique& technique)
{
	m_techniques.push_back(technique);
}

UINT32 Shape::GetIndexCount() const noexcept
{
	return m_pIndexBuffer->GetCount();
}