#include "SkyboxTransformBuffer.h"
#include "Shape.h"
#include "Camera.h"

SkyboxTransformBuffer::SkyboxTransformBuffer(GFX& gfx, const Shape& parent, std::vector<TargetShaderBufferBinding> targetBuffers)
	:
	TransformConstBuffer(gfx, parent, targetBuffers)
{

}

void SkyboxTransformBuffer::GetBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData) const noexcept
{
	const auto cameraViewProjection = gfx.GetActiveCamera()->GetProjectionView();

	bufferData += DirectX::XMMatrixTranspose(cameraViewProjection);
}

DynamicConstantBuffer::BufferLayout SkyboxTransformBuffer::GetBufferLayout() const noexcept
{
	return { "MA" };
}