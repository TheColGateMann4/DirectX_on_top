#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "BindableList.h"
#include "Vertex.h"

class Texture : public Bindable
{
public:
	Texture(GFX& gfx, const std::string imagePath, UINT32 slot = 0, bool isCube = false, TargetShader targetShader = TargetPixelShader);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<Texture> GetBindable(GFX& gfx, const std::string imagePath, UINT32 slot = 0, bool isCube = false, TargetShader targetShader = TargetPixelShader)
	{
		return BindableList::GetBindable<Texture>(gfx, imagePath, slot, isCube, targetShader);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_imagePath, m_slot, m_isCube, m_targetShader);
	};

	static std::string GetStaticUID(const std::string imagePath, UINT32 slot = 0, bool isCube = false, TargetShader targetShader = TargetPixelShader) noexcept
	{
		return GenerateUID(imagePath, slot, isCube, targetShader);
	};

	bool HasAlpha() const
	{
		return m_hasAlpha;
	}

	DXGI_FORMAT GetTextureFormat() const
	{
		return m_textureFormat;
	}

private:
	static std::string GenerateUID(const std::string& imagePath, UINT32 slot = 0, bool isCube = false, TargetShader targetShader = TargetPixelShader)
	{
		return imagePath + std::to_string(slot) + std::to_string(isCube) + std::to_string(targetShader);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	std::string m_imagePath;
	DXGI_FORMAT m_textureFormat;
	UINT32 m_slot;
	bool m_isCube;
	bool m_hasAlpha = false;
	TargetShader m_targetShader;
};

