#include "SceneVisibilityManager.h"
#include "ComputeShader.h"
#include "Camera.h"
#include "Scene.h"

SceneVisibilityManager::SceneVisibilityManager(GFX& gfx)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pModelCubeRWBuffer;

	{
		constexpr UINT32 bufferSize = startingBufferElementWidth * 6;
		std::vector<float> pData;

		pData.resize(bufferSize);

		for(UINT32 i = 0; i < bufferSize / 6; i++)
		{
			pData.at(i * 6) = FLT_MAX;
			pData.at(i * 6 + 1) = FLT_MAX;
			pData.at(i * 6 + 2) = FLT_MAX;

			pData.at(i * 6 + 3) = -FLT_MAX;
			pData.at(i * 6 + 4) = -FLT_MAX;
			pData.at(i * 6 + 5) = -FLT_MAX;
		}

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = bufferSize * sizeof(float); // dynamicly change the size of this buffer when object gets added
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags = NULL;
		bufferDesc.MiscFlags = NULL;
		bufferDesc.StructureByteStride = sizeof(float);

		D3D11_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pSysMem = pData.data();

		THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pModelCubeRWBuffer));
	}

	m_pModelBoundsUAV = ShaderUnorderedAccessView::GetBindable(gfx, 0, pModelCubeRWBuffer, DXGI_FORMAT_R32_FLOAT);

	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = startingBufferElementWidth * sizeof(DirectX::XMMATRIX); // dynamicly change the size of this buffer when object gets added
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags = NULL;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = sizeof(DirectX::XMMATRIX);

		THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, nullptr, &m_pObjectsMatrixBuffer));
	}
}

void SceneVisibilityManager::ProcessVisibilityBuffer(GFX& gfx, Camera* camera, UINT highestIndexOnScene, std::vector<UINT8>* objectValidity)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pCleanModelRWBuffer;


	// Step 2. cube bounds by camera projection view matrix
	{
		// unbinding cube UAV
		{
			ID3D11UnorderedAccessView* pUnorderedAccessView = nullptr;

			GFX::GetDeviceContext(gfx)->CSSetUnorderedAccessViews(0, 1, &pUnorderedAccessView, NULL);
		}

		// making another resource to copy from UAV cube bounds to it and pass it further
		{
			{
				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = 10 * 6 * sizeof(float);
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
				bufferDesc.CPUAccessFlags = NULL;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(float);

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, nullptr, &pCleanModelRWBuffer));
			}

			// copying from our previous UAV to new one to avioid multiplying same resource by matrices couple times
			THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->CopyResource(pCleanModelRWBuffer.Get(), m_pModelBoundsUAV->GetResource()));

			ShaderUnorderedAccessView::GetBindable(gfx, 0, pCleanModelRWBuffer, DXGI_FORMAT_R32_FLOAT)->Bind(gfx);
		}

		// binding camera projection view matrix as buffer
		{
			//			cbuffer cameraData : register(b0)
			//			{
			//				matrix cameraViewProjection;
			//			};

			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;

			{
				DirectX::XMMATRIX pData = DirectX::XMMatrixTranspose(camera->GetProjectionView());

				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = sizeof(pData);
				bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
				bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufferDesc.CPUAccessFlags = NULL;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(pData);


				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = &pData;

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pBuffer));
			}

			GFX::GetDeviceContext(gfx)->CSSetConstantBuffers(0, 1, pBuffer.GetAddressOf());
		}

		//binding validity buffer
		{
			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;

			{
				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = highestIndexOnScene * sizeof(UINT8);
				bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
				bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				bufferDesc.CPUAccessFlags = NULL;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(UINT8);

				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = objectValidity->data();

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pBuffer));
			}

			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pBufferView;

			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
				shaderResourceViewDesc.Format = DXGI_FORMAT_R8_UINT;
				shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
				shaderResourceViewDesc.Buffer.FirstElement = 0;
				shaderResourceViewDesc.Buffer.NumElements = highestIndexOnScene;

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(pBuffer.Get(), &shaderResourceViewDesc, &pBufferView));
			}

			THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->CSSetShaderResources(2, 1, pBufferView.GetAddressOf()));
		}

		//binding buffer of transforms
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pBufferView;

			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
				shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
				shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
				shaderResourceViewDesc.Buffer.FirstElement = 0;
				shaderResourceViewDesc.Buffer.NumElements = highestIndexOnScene;

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(m_pObjectsMatrixBuffer.Get(), &shaderResourceViewDesc, &pBufferView));
			}

			THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->CSSetShaderResources(0, 1, pBufferView.GetAddressOf()));
		}

		{
			ComputeShader::GetBindable(gfx, "CS_ProcessVertices.cso")->Bind(gfx);
		}

		{
			gfx.Dispatch({ 1,1,1 });
		}
	}

	// Step 3. checking if said cube is inside view frustum
	{
		//		cbuffer SceneData : register(b0)
		//		{
		//			uint numberOfModels;
		//		}

		{
			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;

			{
				UINT32 pData[4] =
				{
					highestIndexOnScene,
					0,	//padding
					0,
					0
				};

				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = sizeof(pData);
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufferDesc.CPUAccessFlags = NULL;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(UINT32);

				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = pData;

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pBuffer));
			}

			GFX::GetDeviceContext(gfx)->CSSetConstantBuffers(0, 1, pBuffer.GetAddressOf());
		}

		// Binding our output - bool and overriding uav slot 0 to bind cube resource as buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> pRWOutputBuffer;
		{
			{
				if (m_visiblityData.size() != highestIndexOnScene * 4)
					m_visiblityData.resize(highestIndexOnScene * 4);

				memset(&m_visiblityData[0], FALSE, highestIndexOnScene * 4);

				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = highestIndexOnScene * sizeof(UINT32);
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(UINT32);

				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = &m_visiblityData[0];

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pRWOutputBuffer));
			}

			ShaderUnorderedAccessView::GetBindable(gfx, 0, pRWOutputBuffer, DXGI_FORMAT_R32_UINT)->Bind(gfx);
		}

		// Binding our cube as buffer
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pBufferView;

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
			shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
			shaderResourceViewDesc.Buffer.FirstElement = 0;
			shaderResourceViewDesc.Buffer.NumElements = highestIndexOnScene * 6;

			THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(pCleanModelRWBuffer.Get(), &shaderResourceViewDesc, &pBufferView));

			THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->CSSetShaderResources(0, 1, pBufferView.GetAddressOf()));
		}

		// Binding view frustum as buffer
		{
			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;

			{
				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = camera->GetFrustumBufferByteSize();
				bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
				bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				bufferDesc.CPUAccessFlags = NULL;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(float);

				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = camera->GetFrustumBuffer();

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pBuffer));
			}

			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pBufferView;

			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
				shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
				shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
				shaderResourceViewDesc.Buffer.FirstElement = 0;
				shaderResourceViewDesc.Buffer.NumElements = 8 * 3;// 8 positions made up from 3 floats

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(pBuffer.Get(), &shaderResourceViewDesc, &pBufferView));
			}

			THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->CSSetShaderResources(1, 1, pBufferView.GetAddressOf()));
		}

		{
			ComputeShader::GetBindable(gfx, "CS_FrustumChecking.cso")->Bind(gfx);
		}

		{
			gfx.Dispatch({ 1,1,1 });
		}

		// reading out our return value
		{
			D3D11_MAPPED_SUBRESOURCE subresourceData = {};

			THROW_GFX_IF_FAILED(GFX::GetDeviceContext(gfx)->Map(
				pRWOutputBuffer.Get(),
				0,
				D3D11_MAP_READ,
				NULL,
				&subresourceData
			));

			BOOL* resultData = static_cast<BOOL*>(subresourceData.pData); // we could maybe change to bool to not read 3 empty bytes, but gotta debug

			for(size_t i = 0; i < highestIndexOnScene; i++)
				m_visiblityData.at(i) = resultData[i]; // BOOL to bool

			initializedVector = true;

			GFX::GetDeviceContext(gfx)->Unmap(
				pRWOutputBuffer.Get(),
				0
			);
		}
	}
}

bool SceneVisibilityManager::GetVisibility(size_t objectID) const
{
	if (!initializedVector)
		THROW_INTERNAL_ERROR("Tried to get visibility data before calling update function");

	return m_visiblityData.at(objectID);
}

ShaderUnorderedAccessView* SceneVisibilityManager::GetCubeBoundsUAV() const
{
	return m_pModelBoundsUAV.get();
}

ID3D11Buffer* SceneVisibilityManager::GetMatrixBuffer() const
{
	return m_pObjectsMatrixBuffer.Get();
}
