#pragma once 
#include "Includes.h"
#include "BindableList.h"
#include "Bindable.h"
#include "wrl_no_warnings.h"

class GFX;

class CubeMapTexture : public Bindable
{
public:
	CubeMapTexture(GFX& gfx, const std::string imagePath, UINT32 slot = 0);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<CubeMapTexture> GetBindable(GFX& gfx, const std::string imagePath, UINT32 slot = 0)
	{
		return BindableList::GetBindable<CubeMapTexture>(gfx, imagePath, slot);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_imagePath, m_slot);
	};

	static std::string GetStaticUID(const std::string imagePath, UINT32 slot = 0) noexcept
	{
		return GenerateUID(imagePath, slot);
	};

private:
	static std::string GenerateUID(const std::string& imagePath, UINT32 slot = 0)
	{
		return imagePath + std::to_string(slot);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	std::string m_imagePath;
	UINT32 m_slot;
};
