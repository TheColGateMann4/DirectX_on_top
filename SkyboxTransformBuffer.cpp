#include "SkyboxTransformBuffer.h"
#include "Shape.h"
#include "Camera.h"

SkyboxTransformBuffer::SkyboxTransformBuffer(GFX& gfx, const Shape& parent, UINT32 slot)
	:
	TransformConstBuffer(gfx, parent, slot)
{}

void SkyboxTransformBuffer::Initialize(GFX& gfx, UINT32 slot)
{
	TCBLayout = { "MA" };

	vcbuf = std::make_shared<NonCachedBuffer>(gfx, TCBLayout, slot, false);
}

void SkyboxTransformBuffer::GetBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData) const noexcept
{
	const auto cameraViewProjection = gfx.GetActiveCamera()->GetCameraView() * gfx.GetActiveCamera()->GetProjection();

	bufferData += DirectX::XMMatrixTranspose(cameraViewProjection);
}