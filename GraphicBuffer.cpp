#include "GraphicBuffer.h"
#include "Includes.h"
#include <d3d11.h>
#include <ScreenGrab.h> //SaveWICTextureToFile
#include <wincodec.h> //GUID_ContainerFormatPng
#include "wrl_no_warnings.h"
#include "ErrorMacros.h"
#include "DepthStencilView.h"
#include "Graphics.h"

void GraphicBuffer::SaveToFile(GFX& gfx)
{
	HRESULT hr;

	//DepthStencilView will use D formats which are not supported by the library we use
	bool wicCantProcess = typeid(*this) == typeid(DepthStencilView);

	ID3D11DeviceContext* deviceContext = GFX::GetDeviceContext(gfx);
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	std::string fileName = "SavedImages\\" + std::to_string(m_savedImages) + '_';
	fileName += typeid(*this).name();
	fileName += wicCantProcess ? ".dds" : ".png";

	GetBuffer(&resource);

	if (wicCantProcess)
	{
		THROW_GFX_IF_FAILED(DirectX::SaveDDSTextureToFile(deviceContext, resource.Get(), std::wstring(fileName.begin(), fileName.end()).c_str()));
	}
	else
	{
		THROW_GFX_IF_FAILED(DirectX::SaveWICTextureToFile(deviceContext, resource.Get(), GUID_ContainerFormatPng, std::wstring(fileName.begin(), fileName.end()).c_str()));
	}

	m_savedImages++;
}

size_t GraphicBuffer::m_savedImages = 0;