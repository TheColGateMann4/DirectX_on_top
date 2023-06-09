#pragma  once
#include "Includes.h"
#include <wrl.h>

class GFX
{
	friend class Bindable;

public:
	UINT32 FrameRate = 60;

public:
	VOID Initialize(HWND hWnd);

	VOID FinishFrame();
	VOID ClearBuffer(Vector4f color_);

public:
	VOID DrawIndexed(UINT32 count);

public:
	DirectX::XMMATRIX GetProjection() const noexcept; 
	VOID SetProjection(DirectX::FXMMATRIX projection) noexcept;

private:
	DirectX::XMMATRIX m_projection;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};