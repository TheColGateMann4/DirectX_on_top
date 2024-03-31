#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include <wrl.h>

class NullPixelShader : public Bindable
{
public:
	NullPixelShader(GFX& gfx) {};
	VOID Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<NullPixelShader> GetBindable(GFX& gfx)
	{
		return BindableList::GetBindable<NullPixelShader>(gfx);
	}

	virtual std::string GetLocalUID() const noexcept override
	{
		return GenerateUID();
	};

	static std::string GetStaticUID() noexcept
	{
		return GenerateUID();
	};

private:
	static std::string GenerateUID()
	{
		return {};
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};