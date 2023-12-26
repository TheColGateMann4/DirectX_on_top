#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "BindableList.h"
#include "Vertex.h"

class Texture : public Bindable
{
public:
	Texture(GFX& gfx, const std::string imagePath, UINT32 slot = 0);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<Texture> GetBindable(GFX& gfx, const std::string imagePath, UINT32 slot = 0)
	{
		return BindableList::GetBindable<Texture>(gfx, imagePath, slot);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_imagePath, m_slot);
	};

	static std::string GetStaticUID(const std::string imagePath, UINT32 slot = 0) noexcept
	{
		return GenerateUID(imagePath, slot);
	};

	bool HasAlpha()
	{
		return m_hasAlpha;
	}

private:
	static std::string GenerateUID(const std::string& imagePath, UINT32 slot = 0)
	{
		return imagePath + std::to_string(slot);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	std::string m_imagePath;
	UINT32 m_slot;
	bool m_hasAlpha = false;
};

