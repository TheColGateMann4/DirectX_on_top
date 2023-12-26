#pragma once
#include "Bindable.h"
#include "BindableList.h"

class PixelShader : public Bindable
{
public:
	PixelShader(GFX& gfx, std::string path);
	VOID Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<PixelShader> GetBindable(GFX& gfx, const std::string path)
	{
		return BindableList::GetBindable<PixelShader>(gfx, path);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_path);
	};

	static std::string GetStaticUID(const std::string& path) noexcept
	{
		return GenerateUID(path);
	};

private:
	static std::string GenerateUID(const std::string path)
	{
		return path;
	}

protected:
	std::string m_path;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};