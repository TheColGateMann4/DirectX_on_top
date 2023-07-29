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

	std::string GetUID() const noexcept override
	{
		return GenerateUID(m_imagePath, m_slot);
	};

	static std::string GetUID(const std::string imagePath, UINT32 slot = 0) noexcept
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

