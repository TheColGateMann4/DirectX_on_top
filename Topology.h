#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "BindableList.h"

class Topology : public Bindable
{
public:
	Topology(GFX& gfx, D3D11_PRIMITIVE_TOPOLOGY topologyType);
	VOID Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<Topology> GetBindable(GFX& gfx, D3D11_PRIMITIVE_TOPOLOGY topologyType)
	{
		return BindableList::GetBindable<Topology>(gfx, topologyType);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_topologyType);
	};

	static std::string GetStaticUID(D3D11_PRIMITIVE_TOPOLOGY topologyType) noexcept
	{
		return GenerateUID(topologyType);
	};

private:
	static std::string GenerateUID(D3D11_PRIMITIVE_TOPOLOGY topologyType)
	{
		return std::to_string(topologyType);
	}

protected:
	D3D11_PRIMITIVE_TOPOLOGY m_topologyType;
};

