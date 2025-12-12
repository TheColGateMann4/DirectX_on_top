#include "NullTexture.h"
#include "Graphics.h"

VOID NullTexture::Bind(GFX& gfx) noexcept
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView = nullptr;

	if (m_isPixelShader)
		GFX::GetDeviceContext(gfx)->PSSetShaderResources(m_slot, 1, m_pTextureView.GetAddressOf());
	else
		GFX::GetDeviceContext(gfx)->PSSetShaderResources(m_slot, 1, m_pTextureView.GetAddressOf());
}