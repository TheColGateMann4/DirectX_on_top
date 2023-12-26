#pragma once
#include "Includes.h"
#include "Graphics.h"
#include "Bindable.h"
#include "BindableList.h"

class RasterizerState : public Bindable
{
public:
	RasterizerState(GFX& gfx, bool disableBackfaceCulling);

public:
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<RasterizerState> GetBindable(GFX& gfx, bool disableBackfaceCulling)
	{
		return BindableList::GetBindable<RasterizerState>(gfx, disableBackfaceCulling);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_disableBackfaceCulling);
	};

	static std::string GetStaticUID(bool disableBackfaceCulling) noexcept
	{
		return GenerateUID(disableBackfaceCulling);
	};

private:
	static std::string GenerateUID(bool disableBackfaceCulling)
	{
		return std::to_string(disableBackfaceCulling);
	}

protected:
	bool m_disableBackfaceCulling;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pResterizerState;
};

