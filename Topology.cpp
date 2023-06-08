#include "Topology.h"

Topology::Topology(D3D11_PRIMITIVE_TOPOLOGY topologyType)
	: m_topologyType(topologyType)
{

}

VOID Topology::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->IASetPrimitiveTopology(m_topologyType);
}