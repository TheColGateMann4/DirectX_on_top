#include "SceneVisibilityManager.h"
#include "ComputeShader.h"
#include "Camera.h"
#include "Scene.h"

#define SIZE_OF_CUBE_STRUCTURE (3 * 8)

SceneVisibilityManager::SceneVisibilityManager(GFX& gfx)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pModelCubeRWBuffer;

	{
		constexpr UINT32 bufferSize = startingBufferElementWidth * SIZE_OF_CUBE_STRUCTURE; // vector = 3 floats, and we have 8 vectors
		std::vector<float> pData;

		pData.resize(bufferSize);

		for(UINT32 i = 0; i < bufferSize / SIZE_OF_CUBE_STRUCTURE; i++)
		{
			pData.at(i * SIZE_OF_CUBE_STRUCTURE) = -FLT_MAX;
			pData.at(i * SIZE_OF_CUBE_STRUCTURE + 1) = -FLT_MAX;
			pData.at(i * SIZE_OF_CUBE_STRUCTURE + 2) = -FLT_MAX;

			pData.at(i * SIZE_OF_CUBE_STRUCTURE + 3) = FLT_MAX;
			pData.at(i * SIZE_OF_CUBE_STRUCTURE + 4) = FLT_MAX;
			pData.at(i * SIZE_OF_CUBE_STRUCTURE + 5) = FLT_MAX;

			// we ignore next 3 since they will be overriden by values of previous two, never compared like previous two
		}

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = bufferSize * sizeof(float);
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
		bufferDesc.ByteWidth = startingBufferElementWidth * sizeof(DirectX::XMMATRIX);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags = NULL;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = sizeof(DirectX::XMMATRIX);

		THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, nullptr, &m_pObjectsMatrixBuffer));
	}
}

void SceneVisibilityManager::ProcessVisibilityBuffer(GFX& gfx, Camera* camera, INT32 highestIndexOnScene, std::vector<UINT8>* objectValidity)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pCleanModelRWBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pSceneDataBuffer;
	INT32 numElementsOnScene = highestIndexOnScene + 1;

	THROW_INTERNAL_ERROR_IF("numElementsOnScene was negative value", numElementsOnScene < 0);

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
				bufferDesc.ByteWidth = currentElementWidth * SIZE_OF_CUBE_STRUCTURE * sizeof(float);
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

		// binding number of elements on scene as a buffer
		{
			//		cbuffer sceneData : register(b1)
			//		{
			//			uint numberOfModels;
			//		}
			{
				UINT32 pData[4] =
				{
					numElementsOnScene,
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

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pSceneDataBuffer));
			}

			{
				GFX::GetDeviceContext(gfx)->CSSetConstantBuffers(1, 1, pSceneDataBuffer.GetAddressOf());
			}
		}

		//binding validity buffer
		{
			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;

			{
				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = numElementsOnScene * sizeof(UINT8);
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
				shaderResourceViewDesc.Buffer.NumElements = numElementsOnScene;

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
				shaderResourceViewDesc.Buffer.NumElements = numElementsOnScene;

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
		// Binding the previously created buffer that contains number of objects on scene
		//		cbuffer SceneData : register(b0)
		//		{
		//			uint numberOfModels;
		//		}
		{
			GFX::GetDeviceContext(gfx)->CSSetConstantBuffers(0, 1, pSceneDataBuffer.GetAddressOf());
		}

		// Binding our output - bool and overriding uav slot 0 to bind cube resource as buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> pRWOutputBuffer;
		{
			{
				if (m_visiblityData.size() != numElementsOnScene * 4)
					m_visiblityData.resize(numElementsOnScene * 4);

				memset(&m_visiblityData[0], FALSE, numElementsOnScene * 4);

				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = numElementsOnScene * sizeof(UINT32);
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
			shaderResourceViewDesc.Buffer.NumElements = numElementsOnScene * 8 * 3;

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
				bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				bufferDesc.StructureByteStride = sizeof(DirectX::XMFLOAT3);

				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = camera->GetFrustumBuffer();

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pBuffer));
			}

			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pBufferView;

			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
				shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
				shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
				shaderResourceViewDesc.Buffer.FirstElement = 0;
				shaderResourceViewDesc.Buffer.NumElements = 8; // 8 float3 in structured buffer

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

			for(size_t i = 0; i < numElementsOnScene; i++)
				m_visiblityData.at(i) = resultData[i]; // BOOL to bool

			initializedVector = true;

			GFX::GetDeviceContext(gfx)->Unmap(
				pRWOutputBuffer.Get(),
				0
			);
		}
	}
}

void SceneVisibilityManager::UpdateTransformBuffer(GFX& gfx)
{
	THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->UpdateSubresource(m_pObjectsMatrixBuffer.Get(), 0, NULL, m_modelsMatrixData.data(), m_modelsMatrixData.size() * sizeof(DirectX::XMMATRIX), 0));
}

std::vector<DirectX::XMMATRIX>& SceneVisibilityManager::GetMatrixBuffer()
{
	return m_modelsMatrixData;
}

void SceneVisibilityManager::ResizeBuffers(GFX& gfx, INT32 newHighestObjectID)
{
	// our buffers on GPU will be less affected by small changes
	if(newHighestObjectID > currentElementWidth)
	{
		HRESULT hr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pNewCubeBoundBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pNewTransformMatrixBuffer;

		// first creating new buffers
		{
			{
				const UINT32 bufferSize = (newHighestObjectID + startingBufferElementWidth) * SIZE_OF_CUBE_STRUCTURE;
				std::vector<float> pData;

				pData.resize(bufferSize);

				for (UINT32 i = currentElementWidth; i < bufferSize / SIZE_OF_CUBE_STRUCTURE; i++)
				{
					pData.at(i * SIZE_OF_CUBE_STRUCTURE) = -FLT_MAX;
					pData.at(i * SIZE_OF_CUBE_STRUCTURE + 1) = -FLT_MAX;
					pData.at(i * SIZE_OF_CUBE_STRUCTURE + 2) = -FLT_MAX;

					pData.at(i * SIZE_OF_CUBE_STRUCTURE + 3) = FLT_MAX;
					pData.at(i * SIZE_OF_CUBE_STRUCTURE + 4) = FLT_MAX;
					pData.at(i * SIZE_OF_CUBE_STRUCTURE + 5) = FLT_MAX;
				}

				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = bufferSize * sizeof(float);
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
				bufferDesc.CPUAccessFlags = NULL;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(float);

				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = pData.data();

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pNewCubeBoundBuffer));
			}

			{
				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = (newHighestObjectID + startingBufferElementWidth) * sizeof(DirectX::XMMATRIX);
				
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				bufferDesc.CPUAccessFlags = NULL;
				bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				bufferDesc.StructureByteStride = sizeof(DirectX::XMMATRIX);

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, nullptr, &pNewTransformMatrixBuffer));
			}
		}

		// copying existing resources to them
		{
			D3D11_BOX dataBox = {};
			dataBox.top = dataBox.left = dataBox.front = 0;
			dataBox.bottom = dataBox.back = 1; // just to pass a check
			dataBox.right = currentElementWidth * SIZE_OF_CUBE_STRUCTURE * sizeof(float);

			THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->CopySubresourceRegion(pNewCubeBoundBuffer.Get(), 0, 0, 0, 0, m_pModelBoundsUAV->GetResource(), 0, &dataBox));


			dataBox.right = currentElementWidth * sizeof(DirectX::XMMATRIX);

			THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->CopySubresourceRegion(pNewTransformMatrixBuffer.Get(), 0, 0, 0, 0, m_pObjectsMatrixBuffer.Get(), 0, &dataBox));
		}

		//setting new stuff
		m_pModelBoundsUAV->UpdateResource(gfx, pNewCubeBoundBuffer);
		m_pObjectsMatrixBuffer = pNewTransformMatrixBuffer;

		currentElementWidth = newHighestObjectID + startingBufferElementWidth;
	}

	// this vector will be exact size we want since its not a problem in resizing it
	m_modelsMatrixData.resize(newHighestObjectID);
}

std::vector<UINT8>& SceneVisibilityManager::GetVisibilityBuffer()
{
	return m_visiblityData;
}

ShaderUnorderedAccessView* SceneVisibilityManager::GetCubeBoundsUAV() const
{
	return m_pModelBoundsUAV.get();
}