#include "Renderer/Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer

      Summary:  Constructor

      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                  m_immediateContext, m_immediateContext1, m_swapChain,
                  m_swapChain1, m_renderTargetView, m_vertexShader,
                  m_pixelShader, m_vertexLayout, m_vertexBuffer].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Renderer definition (remove the comment)
    --------------------------------------------------------------------*/
    Renderer::Renderer()
        :m_driverType(D3D_DRIVER_TYPE_HARDWARE),
        m_featureLevel(D3D_FEATURE_LEVEL_11_1),
        m_d3dDevice(nullptr),
        m_d3dDevice1(nullptr),
        m_immediateContext(nullptr),
        m_immediateContext1(nullptr),
        m_swapChain(nullptr),
        m_swapChain1(nullptr),
        m_renderTargetView(nullptr),
        m_vertexShader(nullptr),
        m_pixelShader(nullptr),
        m_vertexLayout(nullptr),
        m_vertexBuffer(nullptr)
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize

      Summary:  Creates Direct3D device and swap chain

      Args:     HWND hWnd
                  Handle to the window

      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                  m_d3dDevice1, m_immediateContext1, m_swapChain1,
                  m_swapChain, m_renderTargetView, m_vertexShader,
                  m_vertexLayout, m_pixelShader, m_vertexBuffer].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/

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
        desc.SampleDesc.Quality = 0;    
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

        //==============================================
        
        //Compile the vertex shader 
        ComPtr<ID3DBlob> pVSBlob(nullptr);
        hr = compileShaderFromFile(L"../Library/Shaders/Lab03.fxh", "VS", "vs_5_0", &pVSBlob);

        if (FAILED(hr))
        {
            MessageBox(nullptr,
                L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }

        //Create the vertex shader
        hr = m_d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
        if (FAILED(hr))
            return hr;

        //Define the input layout
        D3D11_INPUT_ELEMENT_DESC aLayouts[] =
        {
            {"POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        };

        UINT uNumElements = ARRAYSIZE(aLayouts);

        //Create the input layout
        hr = m_d3dDevice->CreateInputLayout(aLayouts, uNumElements, pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(), m_vertexLayout.GetAddressOf());

        if (FAILED(hr))
            return hr;

        //Set input layout???????????
        m_immediateContext->IASetInputLayout(m_vertexLayout.Get());


        //Compile the pixel shader
        ComPtr<ID3DBlob> pPSBlob(nullptr);
        hr = compileShaderFromFile(L"../Library/Shaders/Lab03.fxh", "PS", "ps_5_0", &pPSBlob);

        if (FAILED(hr))
        {
            MessageBox(nullptr,
                L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }

        //Create the pixel shader
        hr = m_d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pixelShader);

        if (FAILED(hr))
            return hr;

        //Create a Vertex Buffer
        SimpleVertex aVertices[] =
        {
            { XMFLOAT3( 0.0f, 0.5f, 0.5f ) },
            { XMFLOAT3( 0.5f, -0.5f, 0.5f ) },
            { XMFLOAT3( -0.5f, -0.5f, 0.5f ) },
        };

        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * 3;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData = {};

        initData.pSysMem = aVertices;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;

        hr = m_d3dDevice->CreateBuffer(&bd, &initData, &m_vertexBuffer);
        
        if (FAILED(hr))
            return hr;

        //Set vertex Buffer
        UINT uStride = sizeof(SimpleVertex);
        UINT uOffset = 0;
        m_immediateContext->IASetVertexBuffers(0u, 1u, &m_vertexBuffer, &uStride, &uOffset);

        //Set topology
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        //==============================================================


        ////Create an Index Buffer
        //ComPtr<ID3D11Buffer> pIndexBuffer(nullptr);

        //WORD aIndices[] =
        //{
        //    0,1,2
        //};

        //D3D11_BUFFER_DESC bd2 = {};
        //bd2.Usage = D3D11_USAGE_DEFAULT;
        //bd2.ByteWidth = sizeof(WORD) * 3;
        //bd2.BindFlags = D3D11_BIND_INDEX_BUFFER;
        //bd2.CPUAccessFlags = 0;
        //bd2.MiscFlags = 0;
        //  
        //D3D11_SUBRESOURCE_DATA initData2 = {};

        //initData2.pSysMem = aIndices;
        //initData2.SysMemPitch = 0;
        //initData2.SysMemSlicePitch = 0;

        //hr = m_d3dDevice->CreateBuffer(&bd2, &initData2, &pIndexBuffer);

        //if (FAILED(hr))
        //    return hr;

        ////set index buffer
        //m_immediateContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

        //m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Render

      Summary:  Render the frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Render definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::Render()
    {
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::MidnightBlue);

        ////Setting the Shaders
        m_immediateContext->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
        m_immediateContext->PSSetShader(m_pixelShader.Get(), nullptr, 0u);

        m_immediateContext->Draw(3, 0);
        //m_immediateContext->DrawIndexed(3, 0, 0);
        m_swapChain->Present(0, 0);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::compileShaderFromFile

      Summary:  Helper for compiling shaders with D3DCompilec

      Args:     PCWSTR pszFileName
                  A pointer to a constant null-terminated string that
                  contains the name of the file that contains the
                  shader code
                PCSTR pszEntryPoint
                  A pointer to a constant null-terminated string that
                  contains the name of the shader entry point function
                  where shader execution begins
                PCSTR pszShaderModel
                  A pointer to a constant null-terminated string that
                  specifies the shader target or set of shader
                  features to compile against
                ID3DBlob** ppBlobOut
                  A pointer to a variable that receives a pointer to
                  the ID3DBlob interface that you can use to access
                  the compiled code

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::compileShaderFromFile definition (remove the comment)
    --------------------------------------------------------------------*/

    HRESULT Renderer::compileShaderFromFile(_In_ PCWSTR pszFileName, _In_ PCSTR pszEntryPoint, _In_ PCSTR szShaderModel, _Outptr_ ID3DBlob** ppBlobOut)
    {
        HRESULT hr = S_OK;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined (DEBUG) || defined (_DEBUG)
        dwShaderFlags |= D3DCOMPILE_DEBUG;
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif     

        ComPtr<ID3DBlob> pErrorBlob(nullptr);
        hr = D3DCompileFromFile(pszFileName, nullptr, nullptr, pszEntryPoint, szShaderModel, dwShaderFlags, 0u, ppBlobOut, pErrorBlob.GetAddressOf());

        if (FAILED(hr))
        {
            if (pErrorBlob)
            {
                OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
                pErrorBlob.Reset();
            }
            return hr;
        }
        if (pErrorBlob) pErrorBlob.Reset();

        return S_OK;
    }
}