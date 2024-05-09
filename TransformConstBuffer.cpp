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

		for (const auto& targetShader : m_targetBuffers)
			constBuffers.push_back(std::make_shared<NonCachedBuffer>(gfx, TCBLayout, targetShader.slot, targetShader.type));

		initialized = true;
	}

	DynamicConstantBuffer::BufferData bufferData(TCBLayout);

	GetBuffer(gfx, bufferData);

	UpdateAndBindConstBuffer(gfx, bufferData);
}

void TransformConstBuffer::UpdateAndBindConstBuffer(GFX& gfx, const DynamicConstantBuffer::BufferData& bufferData) noexcept
{
	for(auto& constBuffer : constBuffers)
	{
		constBuffer->Update(gfx, bufferData);
		constBuffer->Bind(gfx);
	}
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