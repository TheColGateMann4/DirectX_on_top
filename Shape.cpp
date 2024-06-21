#include "Shape.h"
#include "BindableClassesMacro.h"
#include "RenderJob.h"

#include "ShaderUnorderedAccessView.h"
#include "ComputeShader.h"
#include "Camera.h"


void Shape::Render() const noexcept(!IS_DEBUG)
{
	for (const auto& technique : m_techniques)
		technique.Render(this);
}

void Shape::LinkToPipeline(class RenderGraph& renderGraph)
{
	for (auto& technique : m_techniques)
		technique.LinkToPipeline(renderGraph);
}

void Shape::Bind(GFX& gfx) const noexcept
{
	m_pIndexBuffer->Bind(gfx);

	m_pTransformConstBuffer->Bind(gfx);

	m_pVertexBuffer->Bind(gfx);

	if(m_pTopology)
		m_pTopology->Bind(gfx);
}

bool Shape::CheckIfVisible(GFX& gfx, Camera* camera)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pModelCubeRWBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pFrustumRWBuffer;
	constexpr UINT32 uavBufferSize = 6;

	// Step 1. Getting cube positions that contain our model
	{
		// UAV setup
		{
			{
				float pData[6] = 
				{
					FLT_MAX,
					FLT_MAX,
					FLT_MAX,
					-FLT_MAX,
					-FLT_MAX,
					-FLT_MAX,
				};

				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = uavBufferSize * sizeof(float);
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
				bufferDesc.CPUAccessFlags = NULL;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(float);

				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = pData;

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pModelCubeRWBuffer));
			}

			ShaderUnorderedAccessView::GetBindable(gfx, 0, pModelCubeRWBuffer, DXGI_FORMAT_R32_FLOAT)->Bind(gfx);
		}

		// giving vertex buffer as input to our shader
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pConstBufferView;

			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
				shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
				shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
				shaderResourceViewDesc.Buffer.FirstElement = 0;
				shaderResourceViewDesc.Buffer.NumElements = m_pVertexBuffer->GetBufferByteSize() / 4;

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(m_pVertexBuffer->Get(), &shaderResourceViewDesc, &pConstBufferView));
			}

			THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->CSSetShaderResources(0, 1, pConstBufferView.GetAddressOf()));
		}

		// binding info about our vertex structure
		{
//			cbuffer verticesInfo : register(b0)
//			{
//				uint positionOffsetInVertexInIndex;
//				uint verticeStructureSize;
//			};

			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;

			{
				const DynamicVertex::VertexLayout& vertexLayout = m_pVertexBuffer->GetLayout();

				UINT pData[4] =
				{
					// in shaders we operate on float arrays, and every thread doesn't need to do the same operation, that's why we divide our values by 4 here
					vertexLayout.GetByIndex(vertexLayout.GetIndexOfElement(DynamicVertex::VertexLayout::VertexComponent::Position3D)).GetOffset() / 4,
					vertexLayout.GetByteSize() / 4,
					0,	//padding
					0
				};

				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = sizeof(pData);
				bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
				bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufferDesc.CPUAccessFlags = NULL;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(UINT);

				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = pData;

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pBuffer));
			}

			GFX::GetDeviceContext(gfx)->CSSetConstantBuffers(0, 1, pBuffer.GetAddressOf());
		}

		//compute shader for making cube
		{
			ComputeShader::GetBindable(gfx, "CS_BoundsCubeOfModel.cso")->Bind(gfx);
		}

		// running compute shader for generating cube that contains model fully
		{
			gfx.Dispatch({ 1,1,1 });
		}
	}

	// Step 2. multiplying frustum and cube bounds by camera projection view matrix
	{

		// we don't need to bind pModelCubeRWBuffer UAV since its still bound, gotta keep that on mind though

		// Binding view frustum as UAV
		{
			{
				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = camera->GetFrustumBufferByteSize();
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
				bufferDesc.CPUAccessFlags = NULL;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(float);

				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = camera->GetFrustumBuffer();

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pFrustumRWBuffer));
			}

			ShaderUnorderedAccessView::GetBindable(gfx, 1, pFrustumRWBuffer, DXGI_FORMAT_R32_FLOAT)->Bind(gfx);
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

		{
			ComputeShader::GetBindable(gfx, "CS_ProcessVertices.cso")->Bind(gfx);
		}

		{
			gfx.Dispatch({ 1,1,1 });
		}
	}

	// Step 3. checking if said cube is inside view frustum
	{
		// Unbinding our uav
		{
			Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUAV = nullptr;

			GFX::GetDeviceContext(gfx)->CSSetUnorderedAccessViews(1, 1, pUAV.GetAddressOf(), NULL); // unbinding frustum uav
		}

		// Binding our output - bool and overriding uav slot 0 to bind cube resource as buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> pRWOutputBuffer;
		{
			{
				UINT pData[1] = { FALSE };

				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = sizeof(UINT); // just single field
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				bufferDesc.MiscFlags = NULL;
				bufferDesc.StructureByteStride = sizeof(UINT);

				D3D11_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pSysMem = pData;

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
			shaderResourceViewDesc.Buffer.NumElements = uavBufferSize;

			THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(pModelCubeRWBuffer.Get(), &shaderResourceViewDesc, &pBufferView));

			THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->CSSetShaderResources(0, 1, pBufferView.GetAddressOf()));
		}

		// Binding view frustum as buffer
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pBufferView;

			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
				shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
				shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
				shaderResourceViewDesc.Buffer.FirstElement = 0;
				shaderResourceViewDesc.Buffer.NumElements = 8 * 3;

				THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(pFrustumRWBuffer.Get(), &shaderResourceViewDesc, &pBufferView));
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
			BOOL result;

			D3D11_MAPPED_SUBRESOURCE subresourceData = {};

			THROW_GFX_IF_FAILED(GFX::GetDeviceContext(gfx)->Map(
				pRWOutputBuffer.Get(),
				0,
				D3D11_MAP_READ,
				NULL,
				&subresourceData
			));

			result = static_cast<BOOL*>(subresourceData.pData)[0];

			GFX::GetDeviceContext(gfx)->Unmap(
				pRWOutputBuffer.Get(),
				0
			);

			return result;
		}
	}
}

void Shape::AddRenderTechnique(const RenderTechnique& technique)
{
	m_techniques.push_back(technique);
}

UINT32 Shape::GetIndexCount() const noexcept
{
	return m_pIndexBuffer->GetCount();
}