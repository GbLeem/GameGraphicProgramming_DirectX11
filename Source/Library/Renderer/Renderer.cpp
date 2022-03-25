#include "Renderer/Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer

      Summary:  Constructor

      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1, 
                  m_immediateContext, m_immediateContext1, m_swapChain, 
                  m_swapChain1, m_renderTargetView].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    Renderer::Renderer()
    {
        m_driverType = D3D_DRIVER_TYPE_HARDWARE;
        m_featureLevel = D3D_FEATURE_LEVEL_11_1;
        m_d3dDevice = nullptr;
        m_d3dDevice1 = nullptr;
        m_immediateContext = nullptr;
        m_immediateContext1 = nullptr;
        m_swapChain = nullptr;
        m_swapChain1 = nullptr;
        m_renderTargetView = nullptr;
    }
   
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize

      Summary:  Creates Direct3D device and swap chain

      Args:     HWND hWnd
                  Handle to the window

      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext, 
                  m_d3dDevice1, m_immediateContext1, m_swapChain1, 
                  m_swapChain, m_renderTargetView].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::Initialize(_In_ HWND hWnd)
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        DWORD createDeviceFlags = 0;
        
#ifdef DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif // DEBUG 

        //Create Device
        HRESULT hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, nullptr, 0,
            D3D11_SDK_VERSION, &m_d3dDevice, &m_featureLevel, &m_immediateContext);

        if (SUCCEEDED(m_d3dDevice.As(&m_d3dDevice1)))
        {
            (void)m_immediateContext.As(&m_immediateContext1);
        }

        // Obtain DXGI factory from device
        ComPtr<IDXGIFactory1>dxgiFactory;
        {
            ComPtr<IDXGIDevice> dxgiDevice;
            if (SUCCEEDED(m_d3dDevice.As(&dxgiDevice)))
            {
                ComPtr<IDXGIAdapter> adapter;
                if (SUCCEEDED(dxgiDevice->GetAdapter(&adapter)))
                {
                    hr = adapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
                }
            }
        }
        if (FAILED(hr))
            return hr;

        // Create swap chain.
        ComPtr<IDXGIAdapter> adapter;
        ComPtr<IDXGIFactory2> factory;

        DXGI_SWAP_CHAIN_DESC desc;

        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
        desc.Windowed = TRUE;
        desc.BufferCount = 2;
        desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;    //sampleDesc´Â 1,0
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.OutputWindow = hWnd;

        ComPtr<IDXGIDevice2> dxgiDevice;
        m_d3dDevice.As(&dxgiDevice);

        hr = dxgiDevice->GetAdapter(&adapter);

        if (SUCCEEDED(hr))
        {
            adapter->GetParent(IID_PPV_ARGS(&factory));
            hr = factory->CreateSwapChain(m_d3dDevice.Get(), &desc, &m_swapChain);
        }

        //Create Render Target View
        ComPtr<ID3D11Texture2D> g_pBackBuffer;
        hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&g_pBackBuffer));
        if (FAILED(hr))
            return hr;

        hr = m_d3dDevice->CreateRenderTargetView(g_pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf()); 
        g_pBackBuffer.Reset();

        if (FAILED(hr))
            return hr;

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;

        m_immediateContext->RSSetViewports(1, &vp);

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Render

      Summary:  Render the frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::Render()
    {   
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::MidnightBlue); 
        m_swapChain->Present(0, 0);
    }
}