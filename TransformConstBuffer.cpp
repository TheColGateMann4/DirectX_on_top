#pragma once
#include "TransformConstBuffer.h"

TransformConstBuffer::TransformConstBuffer(GFX& gfx, const Shape& parent)
	: m_parent(parent), vcbuf(gfx)
{

}

VOID TransformConstBuffer::Bind(GFX& gfx) noexcept
{
	vcbuf.Update(gfx, DirectX::XMMatrixTranspose(m_parent.GetTranformMatrix() * gfx.GetProjection()));
	vcbuf.Bind(gfx);
}