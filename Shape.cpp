#include "Shape.h"
#include "IndexBuffer.h"
#include "ConstantBuffers.h"
#include "TransformConstBuffer.h"
#include <cassert>
#include <typeinfo>

VOID Shape::Draw(GFX& gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : m_binds)
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(m_pIndexBuffer->GetCount());
}

VOID Shape::AddBindable(std::shared_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	if (typeid(*bind) == typeid(IndexBuffer))
	{
		assert("Attempting to bind Index Buffer second time" && m_pIndexBuffer == NULL);
		m_pIndexBuffer = static_cast<IndexBuffer*>(bind.get());
	}

	m_binds.push_back(std::move(bind));
}

VOID Shape::AddIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) noexcept(!IS_DEBUG)
{
	assert("Attempting to bind Index Buffer second time" && m_pIndexBuffer == NULL);

	m_pIndexBuffer = indexBuffer.get();
	m_binds.push_back(std::move(indexBuffer));
}