#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "BindableList.h"
#include "wrl_no_warnings.h"

class GFX;

class BlendState : public Bindable
{
public:
	BlendState(GFX& gfx, bool blend);

public:
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<BlendState> GetBindable(GFX& gfx, bool blend)
	{
		return BindableList::GetBindable<BlendState>(gfx, blend);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_blend);
	};

	static std::string GetStaticUID(bool blend) noexcept
	{
		return GenerateUID(blend);
	};

private:
	static std::string GenerateUID(bool blend)
	{
		return std::to_string(blend);
	}

protected:
	bool m_blend;
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
};

