#include "ComputeShader.h"
#include "Graphics.h"
#include <d3dcompiler.h>

ComputeShader::ComputeShader(GFX& gfx, std::string path)
	: m_path(path)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

	path = "Shaders\\" + path;

	THROW_GFX_IF_FAILED
	(
		D3DReadFileToBlob
		(
			std::wstring(path.begin(), path.end()).c_str(), &pBlob
		)
	);

	THROW_GFX_IF_FAILED
	(
		GFX::GetDevice(gfx)->CreateComputeShader
		(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			NULL,
			&pComputeShader
		)
	);
}

VOID ComputeShader::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->CSSetShader(pComputeShader.Get(), NULL, NULL);
}