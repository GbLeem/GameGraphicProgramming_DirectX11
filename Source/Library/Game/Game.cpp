#include "Game/Game.h"

namespace library
{
    /*--------------------------------------------------------------------
      Global Variables
    --------------------------------------------------------------------*/
    HINSTANCE                       g_hInst = nullptr;
    HWND                            g_hWnd = nullptr;

    ComPtr<ID3D11Device>            g_pd3dDevice;
    ComPtr<ID3D11Device1>           g_pd3dDevice1;
    ComPtr<ID3D11DeviceContext>     g_pImmediateContext;
    ComPtr<ID3D11DeviceContext1>    g_pImmediateContext1;
    ComPtr<IDXGISwapChain>          g_pSwapChain;
    ComPtr<ID3D11RenderTargetView>  g_pRenderTargetView;
    ComPtr<ID3D11Texture2D>         g_pBackBuffer;
    /*--------------------------------------------------------------------
      Forward declarations
    --------------------------------------------------------------------*/

    /*F+F+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      Function: WindowProc

      Summary:  Defines the behavior of the window—its appearance, how
                it interacts with the user, and so forth

      Args:     HWND hWnd
                  Handle to the window
                UINT uMsg
                  Message code
                WPARAM wParam
                  Additional data that pertains to the message
                LPARAM lParam
                  Additional data that pertains to the message

      Returns:  LRESULT
                  Integer value that your program returns to Windows
    -----------------------------------------------------------------F-F*/
    LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

    LRESULT WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CLOSE:
            if (MessageBox(hWnd, L"Really Quit?", L"Game Graphics Programming", MB_OKCANCEL) == IDOK)
            {
                DestroyWindow(hWnd);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam); 
        }
        return 0;
    }

    HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ INT nCmdShow)
    {
        //register the window class
        WNDCLASSEX wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = PSZ_COURSE_TITLE;
        wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

        if (!RegisterClassEx(&wcex))
        {
            DWORD dwError = GetLastError();

            MessageBox(nullptr, L"Call to RegisterClassEx failed", L"Game Graphics Programming", NULL);

            if (dwError != ERROR_CLASS_ALREADY_EXISTS)
                return HRESULT_FROM_WIN32(dwError);

            return E_FAIL;
        }

        //create window 
        g_hInst = hInstance;
        RECT rc = { 0,0,800,600 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        g_hWnd = CreateWindowEx(0, PSZ_COURSE_TITLE, L"Game Graphics Programming Lab 01: Direct3D 11 Basics",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | CW_USEDEFAULT |WS_MINIMIZEBOX | CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
            nullptr, nullptr, hInstance, nullptr);

        if (!g_hWnd)
        {
            DWORD dwError = GetLastError();

            MessageBox(nullptr, L"Call to CreateWindowEx failed", L"Game Graphics Programming", NULL);

            if (dwError != ERROR_CLASS_ALREADY_EXISTS)
                return HRESULT_FROM_WIN32(dwError);

            return E_FAIL;
        }

        ShowWindow(g_hWnd, nCmdShow);

        return S_OK;
    }

    HRESULT InitDevice()
    {
        D3D_FEATURE_LEVEL lvl[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        D3D_FEATURE_LEVEL fl;

        RECT rc;
        GetClientRect(g_hWnd, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        DWORD createDeviceFlags = 0;

#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 

        //create device
        HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, lvl,
            _countof(lvl), D3D11_SDK_VERSION, &g_pd3dDevice, &fl, &g_pImmediateContext);

        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                createDeviceFlags, &lvl[1], _countof(lvl) - 1,
                D3D11_SDK_VERSION, &g_pd3dDevice, &fl, &g_pImmediateContext);
        }
        
        if (SUCCEEDED(g_pd3dDevice.As(&g_pd3dDevice1)))
        {
            (void)g_pImmediateContext.As(&g_pImmediateContext1);
        }
        
        //obtian DXGI factory from device
        ComPtr<IDXGIFactory1>dxgiFactory;
        {
            ComPtr<IDXGIDevice> dxgiDevice;
            if (SUCCEEDED(g_pd3dDevice.As(&dxgiDevice)))
            {
                ComPtr<IDXGIAdapter> adapter;
                if(SUCCEEDED(dxgiDevice->GetAdapter(&adapter)))
                {
                    hr = adapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
                }
            }
        }
        if (FAILED(hr))
            return hr;

        //create swap chain
        ComPtr<IDXGIAdapter> adapter;
        ComPtr<IDXGIFactory2> factory;

        DXGI_SWAP_CHAIN_DESC desc;

        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
        desc.Windowed = TRUE; 
        desc.BufferCount = 2;
        desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;      
        desc.SampleDesc.Quality = 0;   
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.OutputWindow = g_hWnd;

        ComPtr<IDXGIDevice2> dxgiDevice;
        g_pd3dDevice.As(&dxgiDevice);

        hr = dxgiDevice->GetAdapter(&adapter);

        if (SUCCEEDED(hr))
        {
            adapter->GetParent(IID_PPV_ARGS(&factory));

            hr = factory->CreateSwapChain(g_pd3dDevice.Get(), &desc, &g_pSwapChain);
        }

        //create render target view
        hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&g_pBackBuffer));
        if (FAILED(hr))
            return hr;
        
        //use GetAddressOf 
        hr = g_pd3dDevice->CreateRenderTargetView(g_pBackBuffer.Get(), nullptr, g_pRenderTargetView.GetAddressOf()); //device
        if (FAILED(hr))
            return hr;
        
        g_pImmediateContext->OMSetRenderTargets(1, g_pRenderTargetView.GetAddressOf(), nullptr); //context
        

        //setup viewport
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;
        
        g_pImmediateContext->RSSetViewports(1, &vp); //by context
        
        return S_OK;
    }

    void CleanupDevice()
    {
        if (g_pd3dDevice) g_pd3dDevice.Reset();
        if(g_pd3dDevice1) g_pd3dDevice1.Reset();
        if(g_pImmediateContext) g_pImmediateContext.Reset();
        if(g_pImmediateContext1) g_pImmediateContext1.Reset();
        if(g_pSwapChain) g_pSwapChain.Reset();
        if(g_pRenderTargetView) g_pRenderTargetView.Reset();
        if(g_pBackBuffer) g_pBackBuffer.Reset();
    }

    void Render()
    {
        //use Get()
        g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView.Get(), Colors::MidnightBlue); 
        g_pSwapChain->Present(0, 0); //swap (back <-> front)
    }
}