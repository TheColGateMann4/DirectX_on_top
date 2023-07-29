#pragma once
#include "Bindable.h"
#include "BindableList.h"

class VertexShader : public Bindable
{
public:
	VertexShader(GFX& gfx, const std::string& path);
	VertexShader(GFX& gfx, const std::string&& path);
	VOID Bind(GFX& gfx) noexcept;
	ID3DBlob* GetByteCode() const noexcept;

public:
	static std::shared_ptr<VertexShader> GetBindable(GFX& gfx, const std::string& path)
	{
		return BindableList::GetBindable<VertexShader>(gfx, path);
	}

	std::string GetUID() const noexcept override
	{
		return GenerateUID(m_path);
	};

	static std::string GetUID(const std::string& path) noexcept
	{
		return GenerateUID(path);
	};

private:
	static std::string GenerateUID(const std::string& path)
	{
		return path;
	}

protected:
	std::string m_path;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
};