#include "NullShader.h"
#include "Graphics.h"
#include <d3dcompiler.h>

NullShader::NullShader(GFX& gfx, TargetShader targetShader)
	:
	m_targetShader(targetShader)
{

}

VOID NullShader::Bind(GFX& gfx) noexcept
{
	if (m_targetShader == TargetPixelShader)
		GFX::GetDeviceContext(gfx)->PSSetShader(nullptr, NULL, NULL);
	else if (m_targetShader == TargetVertexShader)
		GFX::GetDeviceContext(gfx)->VSSetShader(nullptr, NULL, NULL);
	else if (m_targetShader == TargetHullShader)
		GFX::GetDeviceContext(gfx)->HSSetShader(nullptr, NULL, NULL);
	else if (m_targetShader == TargetDomainShader)
		GFX::GetDeviceContext(gfx)->DSSetShader(nullptr, NULL, NULL);
}