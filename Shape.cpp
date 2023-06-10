#include "Shape.h"
#include "IndexBuffer.h"
#include "TransformConstBuffer.h"
#include <cassert>
#include <typeinfo>

VOID Shape::Draw(GFX& gfx) const
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	for (auto& b : GetStaticBindables())
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

VOID Shape::AddBindable(std::unique_ptr<Bindable> bind)
{
	assert("*Must* use AddIndexBuffer to bind Index Buffer" && typeid(bind) != typeid(IndexBuffer));

	binds.push_back(std::move(bind));
}

VOID Shape::AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept
{
	assert("Attempting to bind Index Buffer second time" && pIndexBuffer == NULL);

	pIndexBuffer = indexBuffer.get();
	binds.push_back(std::move(indexBuffer));
}