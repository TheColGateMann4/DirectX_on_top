#pragma  once
#include "Includes.h"
#include "RenderTarget.h"
#include "DepthStencilView.h"
#include "wrl_no_warnings.h"

class Camera;

class GFX
{
	friend class GraphicResource;

public:
	void Initialize(HWND hWnd);
	void SetResolution(UINT32 width, UINT32 height);

	void BeginFrame() const;
	void FinishFrame();

public:
	std::shared_ptr<RenderTarget>* GetRenderTarget();
	std::shared_ptr<DepthStencilView>* GetDepthStencil();

public:
	void ShowImGUI(bool show);
	BOOL isImGUIVisible() const;

public:
	void DrawIndexed(UINT32 count);

public:
	Camera* GetActiveCamera() const;

	void SetActiveCameraLinkage(Camera** ppActiveCamera);

public:
	UINT32 GetWidth() const { return m_width; };
	UINT32 GetHeight() const { return m_height; };

public:
	static ID3D11DeviceContext* GetDeviceContext(GFX& gfx) noexcept;
	static ID3D11Device* GetDevice(GFX& gfx) noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
	std::shared_ptr<RenderTarget> m_backBuffer;
	std::shared_ptr<DepthStencilView> m_depthStencil;

private:
	UINT32 m_width = 0;
	UINT32 m_height = 0;
	bool m_imgui_enabled = true;
	Camera** m_ppActiveCamera = nullptr;
};