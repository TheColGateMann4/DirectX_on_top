#include "Graphics.h"
#include "DepthStencilView.h"
#include "ErrorMacros.h"

#include <d3dcompiler.h>
#include <cmath>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include "imgui/imgui.h"
#include "imgui/backend/imgui_impl_dx11.h"
#include "imgui/backend/imgui_impl_win32.h"


void GFX::Initialize(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC scDesc = {};
	scDesc.BufferDesc.Width = 0;
	scDesc.BufferDesc.Height = 0;
	scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scDesc.BufferDesc.RefreshRate.Denominator = 0;
	scDesc.BufferDesc.RefreshRate.Numerator = 0;
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = 1;
	scDesc.OutputWindow = hWnd;
	scDesc.Windowed = TRUE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scDesc.Flags = 0;

	HRESULT hr;
	UINT32 SwapChainFlags = 0;

#ifdef _DEBUG
	SwapChainFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	THROW_GFX_IF_FAILED(D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		SwapChainFlags,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scDesc,
		&pSwapChain,
		&pDevice,
		NULL,
		&pDeviceContext

	));

	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;

	THROW_GFX_IF_FAILED(pSwapChain->GetBuffer(
		NULL,
		__uuidof(ID3D11Resource),
		&pBackBuffer
	));

	m_backBuffer = std::make_shared<RenderTarget>(*this, pBackBuffer);
	m_depthStencil = std::make_shared<DepthStencilView>(*this);

	ImGui_ImplDX11_Init(pDevice.Get(), pDeviceContext.Get());
}

void GFX::SetResolution(UINT32 width, UINT32 height)
{
	this->m_width = width;
	this->m_height = height;
}

void GFX::BeginFrame()
{
	//imgui
	if (this->m_imgui_enabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}

void GFX::FinishFrame()
{
	if (this->m_imgui_enabled)
	{
		//ImGui::ShowDemoWindow(&this->m_imgui_enabled);
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT hr;

#ifdef _DEBUG
	DXGImanager.Set();
#endif

	if (FAILED(hr = pSwapChain->Present(1, NULL)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			THROW_GFX_DEVICE_REMOVED(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			THROW_GFX_FAILED(hr);
		}
	}
}

std::shared_ptr<RenderTarget>* GFX::GetRenderTarget()
{
	return &m_backBuffer;
}

std::shared_ptr<DepthStencilView>* GFX::GetDepthStencil()
{
	return &m_depthStencil;
}

void GFX::ShowImGUI(bool show)
{
	this->m_imgui_enabled = show;
}

BOOL GFX::isImGUIVisible() const
{
	return this->m_imgui_enabled;
}

void GFX::DrawIndexed(UINT32 count)
{
	THROW_INFO_EXCEPTION
	(
		pDeviceContext->DrawIndexed
		(
			count,
			0u,
			0u
		)
	);
}

Camera* GFX::GetActiveCamera() const
{
	return *m_ppActiveCamera;
}

void GFX::SetActiveCameraLinkage(Camera** ppActiveCamera)
{
	m_ppActiveCamera = ppActiveCamera;
}