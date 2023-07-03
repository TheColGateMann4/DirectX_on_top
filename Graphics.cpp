#include "Graphics.h"
#include "ErrorMacros.h"

#include <d3dcompiler.h>
#include <cmath>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include "imgui/imgui.h";
#include "imgui/backend/imgui_impl_dx11.h";
#include "imgui/backend/imgui_impl_win32.h";


VOID GFX::Initialize(HWND hWnd)
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

	THROW_GFX_IF_FAILED(
		pDevice->CreateRenderTargetView(
			pBackBuffer.Get(),
			NULL,
			&pTargetView
		));

	D3D11_DEPTH_STENCIL_DESC depthStencilDecs = {};
	depthStencilDecs.DepthEnable = TRUE;
	depthStencilDecs.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDecs.DepthFunc = D3D11_COMPARISON_LESS;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthState;

	THROW_GFX_IF_FAILED(pDevice->CreateDepthStencilState(&depthStencilDecs, &pDepthState));

	pDeviceContext->OMSetDepthStencilState(pDepthState.Get(), 1);


	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;

	D3D11_TEXTURE2D_DESC depthDecs = {};
	depthDecs.Width = this->m_width;
	depthDecs.Height = this->m_height;
	depthDecs.MipLevels = 1;
	depthDecs.ArraySize = 1;
	depthDecs.Format = DXGI_FORMAT_D32_FLOAT;
	depthDecs.SampleDesc.Count = 1;
	depthDecs.SampleDesc.Quality = 0;
	depthDecs.Usage = D3D11_USAGE_DEFAULT;
	depthDecs.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	THROW_GFX_IF_FAILED(pDevice->CreateTexture2D(&depthDecs, NULL, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDecs = {};
	depthStencilViewDecs.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDecs.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDecs.Texture2D.MipSlice = 0;

	THROW_GFX_IF_FAILED(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &depthStencilViewDecs, &pDepthStencilView));

	pDeviceContext->OMSetRenderTargets(1, pTargetView.GetAddressOf(), pDepthStencilView.Get());

	D3D11_VIEWPORT viewPort = {};
	viewPort.Width = this->m_width;
	viewPort.Height = this->m_height;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	pDeviceContext->RSSetViewports(1, &viewPort);

	ImGui_ImplDX11_Init(pDevice.Get(), pDeviceContext.Get());
}

VOID GFX::SetResolution(UINT32 width, UINT32 height)
{
	this->m_width = width;
	this->m_height = height;
}

VOID GFX::BeginFrame(Vector4f color_)
{
	//imgui
	if (this->m_imgui_enabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	//DirectX
	this->ClearBuffer(color_);
}

VOID GFX::FinishFrame()
{
	if (this->m_imgui_enabled)
	{
		ImGui::ShowDemoWindow(&this->m_imgui_enabled);
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

VOID GFX::ClearBuffer(Vector4f color_)
{
	const FLOAT color[] = { color_.x, color_.y, color_.z, color_.t };
	pDeviceContext->ClearRenderTargetView(pTargetView.Get(), color);
	pDeviceContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
}

VOID GFX::ShowImGUI(bool show)
{
	this->m_imgui_enabled = show;
}

BOOL GFX::isImGUIVisible()
{
	return this->m_imgui_enabled;
}

VOID GFX::DrawIndexed(UINT32 count)
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