#pragma  once
#include "Includes.h"
#include "CameraView.h"
#include "RenderQueue.h"
#include <wrl.h>

class GFX
{
	friend class GraphicResource;

public:
	void Initialize(HWND hWnd);
	void SetResolution(UINT32 width, UINT32 height);

	void BeginFrame(DirectX::XMFLOAT4 color = { 0.0f , 0.0f, 0.0f, 1.0f });
	void FinishFrame();

	void ClearBuffer(DirectX::XMFLOAT4 color);

public:
	void BindRenderTarget() const noexcept;
	void BindRenderTarget(const class DepthStencilView& depthStencilView) const noexcept;

public:
	void ShowImGUI(bool show);
	BOOL isImGUIVisible() const;

public:
	void DrawIndexed(UINT32 count);

public:
	UINT32 GetWidth() const { return m_width; };
	UINT32 GetHeight() const { return m_height; };

public:
	CameraView camera;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;

private:
	UINT32 m_width = 0;
	UINT32 m_height = 0;
	bool m_imgui_enabled = true;
};