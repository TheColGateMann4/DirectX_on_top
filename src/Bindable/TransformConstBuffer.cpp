#include "TransformConstBuffer.h"
#include "Camera.h"

TransformConstBuffer::TransformConstBuffer(GFX& gfx, const Shape& parent, std::vector<TargetShaderBufferBinding> targetBuffers)
	:
	m_parent(parent),
	m_targetBuffers(targetBuffers)
{

}

void TransformConstBuffer::Bind(GFX& gfx) noexcept
{
	if (!initialized)
	{
		TCBLayout = GetBufferLayout();

		m_constBuffer = NonCachedBuffer::GetBindable(gfx, TCBLayout, m_targetBuffers);

		initialized = true;
	}

	DynamicConstantBuffer::BufferData bufferData(TCBLayout);

	GetBuffer(gfx, bufferData);

	UpdateAndBindConstBuffer(gfx, bufferData);
}

void TransformConstBuffer::UpdateAndBindConstBuffer(GFX& gfx, const DynamicConstantBuffer::BufferData& bufferData) noexcept
{
	m_constBuffer->Update(gfx, bufferData);
	m_constBuffer->Bind(gfx);
}

void TransformConstBuffer::GetBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData) const noexcept
{
	const auto ModelTransform = m_parent.GetTranformMatrix();
	const auto ModelTransformView = ModelTransform * gfx.GetActiveCamera()->GetCameraView();

	bufferData += DirectX::XMMatrixTranspose(ModelTransform);
	bufferData += DirectX::XMMatrixTranspose(ModelTransformView);
	bufferData += DirectX::XMMatrixTranspose(ModelTransformView * gfx.GetActiveCamera()->GetProjection());
}

DynamicConstantBuffer::BufferLayout TransformConstBuffer::GetBufferLayout() const noexcept
{
	return {"MAMAMA"};
}