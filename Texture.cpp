#include "Texture.h"
#include <DirectXTex.h>

Texture::Texture(GFX& gfx, const std::string imagePath, UINT32 slot, bool uvmap)
	: 
	m_slot(slot),
	m_imagePath(imagePath)
{
	HRESULT hr;
	using namespace DirectX;

	TexMetadata texMetaData = {};
	ScratchImage* textures = new ScratchImage();
	std::wstring wImagePath = std::wstring(imagePath.begin(), imagePath.end());

	THROW_GFX_IF_FAILED(GetMetadataFromWICFile(
		wImagePath.c_str(),
		WIC_FLAGS_ALL_FRAMES,
		texMetaData
	));

	if (uvmap)
	{
		texMetaData.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	THROW_GFX_IF_FAILED(LoadFromWICFile(
		wImagePath.c_str(),
		WIC_FLAGS_ALL_FRAMES,
		&texMetaData,
		*textures
	));

	if(DirectX::HasAlpha(textures->GetImage(0, 0, 0)->format))
	{
		const DirectX::Image *image = textures->GetImages();

		auto imageHasAlpha = [](const DirectX::Image* image)
		{
			for (unsigned int y = 0; y < image->height; y++)
				for (unsigned int x = 0; x < image->width; x++)
					if ((UINT8)image->pixels[((4 * (x + 1)) + image->width * y) - 1] != 255)
						return true;
			return false;
		};

		m_hasAlpha = imageHasAlpha(image);
	}
			
	THROW_GFX_IF_FAILED(CreateShaderResourceView(
		GetDevice(gfx),
		textures->GetImages(),
		textures->GetImageCount(),
		textures->GetMetadata(),
		&pShaderResourceView
	));
}

void Texture::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->PSSetShaderResources(m_slot, 1, pShaderResourceView.GetAddressOf());
}