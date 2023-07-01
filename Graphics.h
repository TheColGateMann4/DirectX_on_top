#pragma  once
#include "Includes.h"
#include <wrl.h>

class GFX
{
	friend class Bindable;

public:
	VOID Initialize(HWND hWnd);
	VOID SetResolution(UINT32 width, UINT32 height);

	VOID FinishFrame();
	VOID ClearBuffer(Vector4f color_ = { 0,0,0,1 });

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

private:
	UINT32 m_width;
	UINT32 m_height;
};