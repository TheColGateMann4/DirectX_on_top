#include "Texture.h"
#include <DirectXTex.h>

Texture::Texture(GFX& gfx, const std::wstring imagePath)
{
	HRESULT hr;
	using namespace DirectX;

	TexMetadata texMetaData = {};
	ScratchImage* textures = new ScratchImage();

	THROW_GFX_IF_FAILED(GetMetadataFromWICFile(
		imagePath.c_str(),
		WIC_FLAGS_ALL_FRAMES,
		texMetaData
	));

	THROW_GFX_IF_FAILED(LoadFromWICFile(
		imagePath.c_str(),
		WIC_FLAGS_ALL_FRAMES,
		&texMetaData,
		*textures
	));

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
	GetDeviceContext(gfx)->PSSetShaderResources(0, 1, pShaderResourceView.GetAddressOf());
}