#include "TransformConstBuffer.h"
#include "Camera.h"

TransformConstBuffer::TransformConstBuffer(GFX& gfx, const Shape& parent, UINT32 slot, TargetShader targetShader)
	:
	m_parent(parent),
	m_targetShader(targetShader)
{
	Initialize(gfx, slot);
}

void TransformConstBuffer::Initialize(GFX& gfx, UINT32 slot)
{
	TCBLayout = { "MAMAMA" };

	vcbuf = std::make_shared<NonCachedBuffer>(gfx, TCBLayout, slot, m_targetShader);
}

void TransformConstBuffer::Bind(GFX& gfx) noexcept
{
	DynamicConstantBuffer::BufferData constBuffer(TCBLayout);

	GetBuffer(gfx, constBuffer);

	UpdateAndBindConstBuffer(gfx, constBuffer);
}

void TransformConstBuffer::UpdateAndBindConstBuffer(GFX& gfx, const DynamicConstantBuffer::BufferData& constBuffer) noexcept
{
	vcbuf->Update(gfx, constBuffer);
	vcbuf->Bind(gfx);
}

void TransformConstBuffer::GetBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData) const noexcept
{
	const auto ModelTransform = m_parent.GetTranformMatrix();
	const auto ModelTransformView = ModelTransform * gfx.GetActiveCamera()->GetCameraView();

	bufferData += DirectX::XMMatrixTranspose(ModelTransform);
	bufferData += DirectX::XMMatrixTranspose(ModelTransformView);
	bufferData += DirectX::XMMatrixTranspose(ModelTransformView * gfx.GetActiveCamera()->GetProjection());
}