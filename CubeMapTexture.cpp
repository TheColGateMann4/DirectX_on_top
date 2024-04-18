#include "CubeMapTexture.h"
#include "Graphics.h"
#include <DirectXTex.h>
#include "ErrorMacros.h"

#define CHECK_METADATA_MEMBER(member) texMetaData.member != localTexMetaData.member

CubeMapTexture::CubeMapTexture(GFX& gfx, const std::string imagePath, UINT32 slot)
	:
	m_slot(slot),
	m_imagePath(imagePath)
{
	HRESULT hr;
	using namespace DirectX;

	std::wstring wImagePath = std::wstring(imagePath.begin(), imagePath.end());

	TexMetadata texMetaData;
	ScratchImage partialImage[6];
	
	{
		size_t startPositionOfExtension = wImagePath.rfind('.');

		if (startPositionOfExtension == std::string::npos)
			std::abort();

		std::wstring baseImagePath = std::wstring(wImagePath.begin(), wImagePath.end() - (wImagePath.size() - startPositionOfExtension));
		const WCHAR* extension = wImagePath.c_str() + startPositionOfExtension;

		constexpr const WCHAR* imageNames[] = { L"_Front", L"_Back", L"_Up", L"_Down", L"_Left", L"_Right" };

		for (size_t i = 0; i <= 5; i++)
		{
			TexMetadata localTexMetaData;
			std::wstring currentTextureName = baseImagePath + std::wstring(imageNames[i]) + extension;

			THROW_GFX_IF_FAILED(GetMetadataFromWICFile(
				currentTextureName.c_str(),
				WIC_FLAGS_NONE,
				localTexMetaData
			));

			if (i == 0)
				texMetaData = localTexMetaData;
			else
				if (CHECK_METADATA_MEMBER(width) ||
					CHECK_METADATA_MEMBER(height) ||
					CHECK_METADATA_MEMBER(depth) ||
					CHECK_METADATA_MEMBER(arraySize) ||
					CHECK_METADATA_MEMBER(mipLevels) ||
					CHECK_METADATA_MEMBER(miscFlags) ||
					CHECK_METADATA_MEMBER(miscFlags2) ||
					CHECK_METADATA_MEMBER(format) ||
					CHECK_METADATA_MEMBER(dimension)
					)
				{
					std::string errorString = "Images weren't compatible. Image had index ";
					errorString += std::to_string(i);
					errorString += ".";

					THROW_INTERNAL_ERROR(errorString.c_str(), true);
				}

			THROW_GFX_IF_FAILED(LoadFromWICFile(
				currentTextureName.c_str(),
				WIC_FLAGS_NONE,
				&localTexMetaData,
				partialImage[i]
			));
		}
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pCubeTexture;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = texMetaData.width;
	textureDesc.Height = texMetaData.height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = texMetaData.format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pCubeTexture));

	for (size_t i = 0; i <= 5; i++)
	{
		THROW_INFO_EXCEPTION(
			GFX::GetDeviceContext(gfx)->UpdateSubresource(
				pCubeTexture.Get(),
				D3D11CalcSubresource(0, i, textureDesc.MipLevels),
				nullptr,
				partialImage[i].GetImages()[0].pixels,
				partialImage[i].GetImages()[0].rowPitch,
				0)
		);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = texMetaData.format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceViewDesc.TextureCube.MipLevels = -1;
	shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;

	THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(pCubeTexture.Get(), &shaderResourceViewDesc, &pShaderResourceView));
}

void CubeMapTexture::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->PSSetShaderResources(m_slot, 1, pShaderResourceView.GetAddressOf());
}

#undef CHECK_METADATA_MEMBER