#include "HullShader.h"
#include "Graphics.h"
#include <d3dcompiler.h>

HullShader::HullShader(GFX& gfx, std::string path)
	: m_path(path)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

#ifdef _DEBUG
	path = "build\\Shaders\\" + path;
#else
	path = "Shaders\\" + path;
#endif

	THROW_GFX_IF_FAILED
	(
		D3DReadFileToBlob
		(
			std::wstring(path.begin(), path.end()).c_str(), &pBlob
		)
	);

	THROW_GFX_IF_FAILED
	(
		GFX::GetDevice(gfx)->CreateHullShader
		(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			NULL,
			&pHullShader
		)
	);
}

VOID HullShader::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->HSSetShader(pHullShader.Get(), NULL, NULL);
}