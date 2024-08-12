#include "Texture.h"
#include "ErrorMacros.h"
#include <DirectXTex.h>

Texture::Texture(GFX& gfx, const std::string imagePath, UINT32 slot, bool isCube, TargetShader targetShader)
	:
	m_imagePath(imagePath),
	m_slot(slot),
	m_isCube(isCube),
	m_targetShader(targetShader)
{
	HRESULT hr;
	using namespace DirectX;

	TexMetadata texMetaData = {};
	ScratchImage textures;
	std::wstring wImagePath = std::wstring(imagePath.begin(), imagePath.end());

	size_t dotPosition = m_imagePath.rfind('.');

	if (dotPosition == std::string::npos)
	{
		std::string errorString = "Could not found extension in texture name. Texture path was: \"";
		errorString += m_imagePath;
		errorString += "\".";
		THROW_INTERNAL_ERROR(errorString.c_str());
	}

	std::string extension = std::string(m_imagePath.begin() + dotPosition + 1, m_imagePath.end());

	for (size_t i = 0; i < extension.length(); i++)
		extension.at(i) = std::tolower(extension.at(i));
	
	if (extension == "tga")
	{
		THROW_GFX_IF_FAILED(GetMetadataFromTGAFile(
			wImagePath.c_str(),
			TGA_FLAGS_NONE,
			texMetaData
		));

		THROW_GFX_IF_FAILED(LoadFromTGAFile(
			wImagePath.c_str(),
			TGA_FLAGS_NONE,
			&texMetaData,
			textures
		));
	}
	else
	{
		THROW_GFX_IF_FAILED(GetMetadataFromWICFile(
			wImagePath.c_str(),
			WIC_FLAGS_NONE,
			texMetaData
		));

		THROW_GFX_IF_FAILED(LoadFromWICFile(
			wImagePath.c_str(),
			WIC_FLAGS_NONE,
			&texMetaData,
			textures
		));
	}

	m_textureFormat = texMetaData.format;
	m_hasAlpha = !textures.IsAlphaAllOpaque();
			
// 	THROW_GFX_IF_FAILED(CreateShaderResourceView(
// 		GetDevice(gfx),
// 		textures->GetImages(),
// 		textures->GetImageCount(),
// 		textures->GetMetadata(),
// 		&pShaderResourceView
// 	));


	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = texMetaData.width;
	textureDesc.Height = texMetaData.height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = texMetaData.format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	if (m_isCube)
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexture));

	GFX::GetDeviceContext(gfx)->UpdateSubresource(pTexture.Get(), 0, nullptr, textures.GetImages()->pixels, textures.GetImages()->rowPitch, 0);

	textures.Release();


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &shaderResourceViewDesc, &pShaderResourceView));

	if(m_targetShader == TargetPixelShader)
		GFX::GetDeviceContext(gfx)->GenerateMips(pShaderResourceView.Get());
}

void Texture::Bind(GFX& gfx) noexcept
{
	if(m_targetShader == TargetPixelShader)
		GFX::GetDeviceContext(gfx)->PSSetShaderResources(m_slot, 1, pShaderResourceView.GetAddressOf());
	else if(m_targetShader == TargetVertexShader)
		GFX::GetDeviceContext(gfx)->VSSetShaderResources(m_slot, 1, pShaderResourceView.GetAddressOf());
	else if (m_targetShader == TargetHullShader)
		GFX::GetDeviceContext(gfx)->HSSetShaderResources(m_slot, 1, pShaderResourceView.GetAddressOf());	
	else if (m_targetShader == TargetDomainShader)
		GFX::GetDeviceContext(gfx)->DSSetShaderResources(m_slot, 1, pShaderResourceView.GetAddressOf());
}