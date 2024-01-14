#include "TransformConstBuffer.h"

TransformConstBuffer::TransformConstBuffer(GFX& gfx, const Shape& parent, UINT32 slot)
	:
	m_parent(parent),
	vcbuf(gfx, TCBLayout, slot, false)
{}

void TransformConstBuffer::Bind(GFX& gfx) noexcept
{
	DynamicConstantBuffer::BufferData constBuffer(TCBLayout);

	GetBuffer(gfx, constBuffer);

	UpdateAndBindConstBuffer(gfx, constBuffer);
}

void TransformConstBuffer::UpdateAndBindConstBuffer(GFX& gfx, const DynamicConstantBuffer::BufferData& constBuffer) noexcept
{
	vcbuf.Update(gfx, constBuffer);
	vcbuf.Bind(gfx);
}

void TransformConstBuffer::GetBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData) const noexcept
{
	const auto ModelTransformView = m_parent.GetTranformMatrix() * gfx.camera.GetCamera();

	bufferData += DirectX::XMMatrixTranspose(ModelTransformView);
	bufferData += DirectX::XMMatrixTranspose(ModelTransformView * gfx.camera.GetProjection());
}