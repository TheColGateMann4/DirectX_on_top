#include "Topology.h"
#include "Graphics.h"

Topology::Topology(GFX& gfx, D3D11_PRIMITIVE_TOPOLOGY topologyType)
	: m_topologyType(topologyType)
{

}

VOID Topology::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->IASetPrimitiveTopology(m_topologyType);
}