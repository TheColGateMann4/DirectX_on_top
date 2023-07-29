#include "Shape.h"
#include "IndexBuffer.h"
#include "PSConstBuffer.h"
#include "ConstantBuffers.h"
#include "TransformConstBuffer.h"
#include <cassert>
#include <typeinfo>

VOID Shape::Draw(GFX& gfx, float time) const
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

VOID Shape::AddBindable(std::shared_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	if (typeid(bind.get()) == typeid(IndexBuffer))
	{
		assert("Attempting to bind Index Buffer second time" && pIndexBuffer == NULL);
		pIndexBuffer = static_cast<const IndexBuffer*>(bind.get());
	}

	binds.push_back(std::move(bind));
}

VOID Shape::AddIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) noexcept(!IS_DEBUG)
{
	assert("Attempting to bind Index Buffer second time" && pIndexBuffer == NULL);

	pIndexBuffer = indexBuffer.get();
	binds.push_back(std::move(indexBuffer));
}