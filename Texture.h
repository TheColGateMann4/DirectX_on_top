#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "BindableList.h"
#include "Vertex.h"

class Texture : public Bindable
{
public:
	Texture(GFX& gfx, const std::string imagePath, UINT32 slot = 0, bool isCube = false);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<Texture> GetBindable(GFX& gfx, const std::string imagePath, UINT32 slot = 0, bool isCube = false)
	{
		return BindableList::GetBindable<Texture>(gfx, imagePath, slot, isCube);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_imagePath, m_slot, m_isCube);
	};

	static std::string GetStaticUID(const std::string imagePath, UINT32 slot = 0, bool isCube = false) noexcept
	{
		return GenerateUID(imagePath, slot, isCube);
	};

	bool HasAlpha()
	{
		return m_hasAlpha;
	}

	DXGI_FORMAT GetTextureFormat()
	{
		return m_textureFormat;
	}

private:
	static std::string GenerateUID(const std::string& imagePath, UINT32 slot = 0, bool isCube = false)
	{
		return imagePath + std::to_string(slot) + std::to_string(isCube);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	std::string m_imagePath;
	DXGI_FORMAT m_textureFormat;
	UINT32 m_slot;
	bool m_isCube;
	bool m_hasAlpha = false;
};

