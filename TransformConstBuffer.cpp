#pragma once
#include "TransformConstBuffer.h"
#include "Application.h"

TransformConstBuffer::TransformConstBuffer(GFX& gfx, const Shape& parent)
	: m_parent(parent), vcbuf(gfx)
{

}

VOID TransformConstBuffer::Bind(GFX& gfx, float programtime) noexcept
{
	waveData wd = { DirectX::XMMatrixTranspose(m_parent.GetTranformMatrix() * gfx.GetProjection()) , programtime };
	vcbuf.Update(gfx,wd);
	vcbuf.Bind(gfx);
}

VOID TransformConstBuffer::Bind(GFX& gfx) noexcept
{

}