#pragma  once
#include "Includes.h"
#include "CameraView.h"
#include <wrl.h>

class GFX
{
	friend class Bindable;

public:
	VOID Initialize(HWND hWnd);
	VOID SetResolution(UINT32 width, UINT32 height);

	VOID BeginFrame(DirectX::XMFLOAT4 color_ = { 0.0f , 0.0f, 0.0f, 1.0f });
	VOID FinishFrame();
	VOID ClearBuffer(DirectX::XMFLOAT4 color_ = { 0.0f, 0.0f, 0.0f, 1.0f });

public:
	VOID ShowImGUI(bool show);
	BOOL isImGUIVisible();

public:
	VOID DrawIndexed(UINT32 count);

public:
	CameraView camera;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;

private:
	UINT32 m_width;
	UINT32 m_height;
	bool m_imgui_enabled = true;
};