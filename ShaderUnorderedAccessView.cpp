#include "ShaderUnorderedAccessView.h"
#include "Graphics.h"
#include <d3dcompiler.h>
#include <DirectXTex.h> // DirectX::BitsPerPixel

ShaderUnorderedAccessView::ShaderUnorderedAccessView(GFX& gfx, UINT32 slot, Microsoft::WRL::ComPtr<ID3D11Resource> pResource, DXGI_FORMAT resourceFormat)
	:
	m_slot(slot),
	m_resourceFormat(resourceFormat)
{
	HRESULT hr;

	pResource.CopyTo(&m_pResource);

	D3D11_RESOURCE_DIMENSION resourceDimension;

	pResource->GetType(&resourceDimension);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = GetUAVDesc(m_pResource, resourceDimension, resourceFormat);

	m_resourceDimension = uavDesc.ViewDimension;

	THROW_GFX_IF_FAILED
	(
		GFX::GetDevice(gfx)->CreateUnorderedAccessView
		(
			m_pResource.Get(),
			&uavDesc,
			&pUnorderedAccessView
		)
	);
}

VOID ShaderUnorderedAccessView::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->CSSetUnorderedAccessViews(m_slot, 1, pUnorderedAccessView.GetAddressOf(), NULL);
}

ID3D11Resource* ShaderUnorderedAccessView::GetResource() const
{
	return m_pResource.Get();
}

constexpr D3D11_UAV_DIMENSION ShaderUnorderedAccessView::GetUAVDimension(const D3D11_RESOURCE_DIMENSION resourceDimension)
{
	switch (resourceDimension)
	{
		case D3D11_RESOURCE_DIMENSION_UNKNOWN:
		{
			return D3D11_UAV_DIMENSION_UNKNOWN;
		}
		case D3D11_RESOURCE_DIMENSION_BUFFER:
		{
			return D3D11_UAV_DIMENSION_BUFFER;
		}
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		{
			return D3D11_UAV_DIMENSION_TEXTURE1D;
		}
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		{
			return D3D11_UAV_DIMENSION_TEXTURE2D;
		}
		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		{
			return D3D11_UAV_DIMENSION_TEXTURE3D;
		}
		default:
		{
			std::string errorString = "wrong resource dimension was used. Dimension number was:";
			errorString += std::to_string(resourceDimension);
			errorString += '.';

			THROW_INTERNAL_ERROR(errorString.c_str());
		}
	}
}

D3D11_UNORDERED_ACCESS_VIEW_DESC ShaderUnorderedAccessView::GetUAVDesc(const Microsoft::WRL::ComPtr<ID3D11Resource>& pResource, const D3D11_RESOURCE_DIMENSION resourceDimension, const DXGI_FORMAT resourceFormat)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC result;

	result.Format = resourceFormat;
	result.ViewDimension = GetUAVDimension(resourceDimension);

	{
		UINT32 bytesPerStructure = DirectX::BitsPerPixel(resourceFormat) / 8;

		switch (resourceDimension)
		{
			case D3D11_RESOURCE_DIMENSION_UNKNOWN:
			{
				THROW_INTERNAL_ERROR("Tried to get UAV desc of resource with unknown dimension.");
			}
			case D3D11_RESOURCE_DIMENSION_BUFFER:
			{
				D3D11_BUFFER_DESC resourceDesc;

				{
					Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;

					pResource->QueryInterface(pBuffer.GetAddressOf());

					pBuffer->GetDesc(&resourceDesc);
				}

				assert(resourceDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS && "Buffer wasn't created with D3D11_BIND_UNORDERED_ACCESS flag");

				result.Buffer.FirstElement = 0;
				result.Buffer.NumElements = resourceDesc.ByteWidth / bytesPerStructure;
				result.Buffer.Flags = NULL;

				break;
			}
			case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			{
				{
					D3D11_TEXTURE1D_DESC resourceDesc;

					{
						Microsoft::WRL::ComPtr<ID3D11Texture1D> pTexture;

						pResource->QueryInterface(pTexture.GetAddressOf());

						pTexture->GetDesc(&resourceDesc);
					}

					assert(resourceDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS && "Texture1D wasn't created with D3D11_BIND_UNORDERED_ACCESS flag");
				}

				result.Texture1D.MipSlice = 0;

				break;
			}
			case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			{
				{
					D3D11_TEXTURE2D_DESC resourceDesc;

					{
						Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

						pResource->QueryInterface(pTexture.GetAddressOf());

						pTexture->GetDesc(&resourceDesc);
					}

					assert(resourceDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS && "Texture2D wasn't created with D3D11_BIND_UNORDERED_ACCESS flag");
				}

				result.Texture2D.MipSlice = 0;

				break;
			}
			case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			{
				{
					D3D11_TEXTURE3D_DESC resourceDesc;

					{
						Microsoft::WRL::ComPtr<ID3D11Texture3D> pTexture;

						pResource->QueryInterface(pTexture.GetAddressOf());

						pTexture->GetDesc(&resourceDesc);
					}

					assert(resourceDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS && "Texture3D wasn't created with D3D11_BIND_UNORDERED_ACCESS flag");
				}

				result.Texture3D.MipSlice = 0;

				break;
			}
			default:
			{
				std::string errorString = "wrong resource dimension was used. Dimension number was:";
				errorString += std::to_string(resourceDimension);
				errorString += '.';

				THROW_INTERNAL_ERROR(errorString.c_str());
			}
		}
	}

	return result;
}