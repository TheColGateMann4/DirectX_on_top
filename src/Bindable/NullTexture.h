#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include "wrl_no_warnings.h"

class NullTexture : public Bindable
{
public:
	NullTexture(GFX& gfx, size_t slot, bool isPixelShader)
		:
		m_slot(slot),
		m_isPixelShader(isPixelShader)
	{};

	VOID Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<NullTexture> GetBindable(GFX& gfx, size_t slot, bool isPixelShader)
	{
		return BindableList::GetBindable<NullTexture>(gfx, slot, isPixelShader);
	}

	virtual std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_slot, m_isPixelShader);
	};

	static std::string GetStaticUID(size_t slot, bool isPixelShader) noexcept
	{
		return GenerateUID(slot, isPixelShader);
	};

private:
	static std::string GenerateUID(size_t slot, bool isPixelShader)
	{
		return std::to_string(slot) + '@' + std::to_string(isPixelShader);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	size_t m_slot;
	bool m_isPixelShader;
};