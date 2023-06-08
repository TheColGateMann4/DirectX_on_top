#pragma once
#include "Includes.h"
#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(D3D11_PRIMITIVE_TOPOLOGY topologyType);
	VOID Bind(GFX& gfx) noexcept override;

protected:
	D3D11_PRIMITIVE_TOPOLOGY m_topologyType;
};

