#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include "wrl_no_warnings.h"

class HullShader : public Bindable
{
public:
	HullShader(GFX& gfx, std::string path);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<HullShader> GetBindable(GFX& gfx, const std::string path)
	{
		return BindableList::GetBindable<HullShader>(gfx, path);
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
	Microsoft::WRL::ComPtr<ID3D11HullShader> pHullShader;
};
