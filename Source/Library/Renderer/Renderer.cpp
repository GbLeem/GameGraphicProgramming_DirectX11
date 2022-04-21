#include "Renderer/Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer

      Summary:  Constructor

      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                 m_immediateContext, m_immediateContext1, m_swapChain,
                 m_swapChain1, m_renderTargetView, m_depthStencil,
                 m_depthStencilView, m_cbChangeOnResize, m_camera,
                 m_projection, m_renderables, m_vertexShaders,
                 m_pixelShaders].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Renderer definition (remove the comment)
    --------------------------------------------------------------------*/
    Renderer::Renderer()
        : m_driverType(D3D_DRIVER_TYPE_HARDWARE)
        , m_featureLevel(D3D_FEATURE_LEVEL_11_1)
        , m_d3dDevice(nullptr)
        , m_d3dDevice1(nullptr)
        , m_immediateContext(nullptr)
        , m_immediateContext1(nullptr)
        , m_swapChain(nullptr)
        , m_swapChain1(nullptr)
        , m_renderTargetView(nullptr)
        , m_depthStencil(nullptr)
        , m_depthStencilView(nullptr)
        , m_cbChangeOnResize()
        , m_cbLights()
        , m_camera(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f))
        , m_projection()
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize

      Summary:  Creates Direct3D device and swap chain

      Args:     HWND hWnd
                  Handle to the window

      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                 m_d3dDevice1, m_immediateContext1, m_swapChain1,
                 m_swapChain, m_renderTargetView, m_cbChangeOnResize,
                 m_projection, m_cbLights, m_camera, m_vertexShaders,
                 m_pixelShaders, m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::Initialize(_In_ HWND hWnd)
    {
        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(hWnd, &rc);

        //fix the mouse cursor inside the client area
        POINT p1, p2;
        p1.x = rc.left;
        p1.y = rc.top;
        p2.x = rc.right;
        p2.y = rc.bottom;

        ClientToScreen(hWnd, &p1);
        ClientToScreen(hWnd, &p2);

        rc.left = p1.x;
        rc.top = p1.y;
        rc.right = p2.x;
        rc.bottom = p2.y;

        ClipCursor(&rc);

        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        UINT createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            m_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());

            if (hr == E_INVALIDARG)
            {
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                    D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());
            }
            if (SUCCEEDED(hr))
                break;
        }
        if (FAILED(hr))
            return hr;

        //Obtain DXGI factory from device 
        ComPtr<IDXGIFactory1> dxgiFactory;
        {
            ComPtr<IDXGIDevice> dxgiDevice;
            hr = m_d3dDevice.As(&dxgiDevice);

            if (SUCCEEDED(hr))
            {
                ComPtr< IDXGIAdapter> adapter;
                hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());

                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
                    adapter.Reset();
                }
                dxgiDevice.Reset();
            }
        }
        if (FAILED(hr))
            return hr;

        //Create swap chain
        ComPtr<IDXGIFactory2> dxgiFactory2(nullptr);

        hr = dxgiFactory.As(&dxgiFactory2);

        if (dxgiFactory2)
        {
            hr = m_d3dDevice.As(&m_d3dDevice1);

            if (SUCCEEDED(hr))
            {
                (void)m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd =
            {
                .Width = width,
                .Height = height,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .SampleDesc = {.Count = 1, .Quality = 0},
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1
            };

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, m_swapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                hr = m_swapChain1.As(&m_swapChain);
            }
            dxgiFactory2.Reset();
        }
        else
        {
            DXGI_SWAP_CHAIN_DESC sd =
            {
                .BufferDesc =
                {
                    .Width = width,
                    .Height = height,
                    .RefreshRate = {.Numerator = 60, .Denominator = 1 },
                    .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                },
                .SampleDesc = {.Count = 1, .Quality = 0},
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1,
                .OutputWindow = hWnd,
                .Windowed = TRUE,
            };

            hr = dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

        dxgiFactory.Reset();

        if (FAILED(hr))
            return hr;

        // Create a render target view
        ComPtr< ID3D11Texture2D> pBackBuffer(nullptr);
        hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));

        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
        pBackBuffer.Reset();

        if (FAILED(hr))
        {
            return hr;
        }

        //m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

        // Setup the viewport
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;

        m_immediateContext->RSSetViewports(1, &vp);

        //<Depth Stencil>  

        //Create a Depth-Stencil buffer 
        D3D11_TEXTURE2D_DESC descDepth =
        {
            .Width = width,
            .Height = height,
            .MipLevels = 1,                          //multisampled texture
            .ArraySize = 1,                          //one texture is in the texture array
            .Format = DXGI_FORMAT_D24_UNORM_S8_UINT, //32bit z buffer format, support 24 bits for depth and 8bits for stencil
            .SampleDesc =                            //default
            {
                .Count = 1,
                .Quality = 0
            },
            .Usage = D3D11_USAGE_DEFAULT,            // resource usage read and write access by GPU
            .BindFlags = D3D11_BIND_DEPTH_STENCIL,   // resource bound as depth-stencil target for the OM stage
            .CPUAccessFlags = 0,                     //no CPU access 
            .MiscFlags = 0,                          //no additional flag
        };

        hr = m_d3dDevice->CreateTexture2D(&descDepth, nullptr, m_depthStencil.GetAddressOf());

        if (FAILED(hr))
            return hr;

        //Create the Depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV =
        {
            .Format = descDepth.Format,                     //Uses the same format of the depth stencil texture
            .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D, //Depth stencil would be accessed as a 2D texture
            .Texture2D =
            {
                .MipSlice = 0                               //The index of the first mipmap level to use is 0
            }
        };

        hr = m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &descDSV, m_depthStencilView.GetAddressOf());

        if (FAILED(hr))
            return hr;

        //Bind Depth-Stencil Data to the OM stage
        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

        //initialize the projection matrix
        m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);

        //initialize the shaders then the renderables
        for (auto Renderableiter : m_renderables)
        {
            Renderableiter.second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
        }
        for (auto VertexShaderiter : m_vertexShaders)
        {
            VertexShaderiter.second->Initialize(m_d3dDevice.Get());
        }
        for (auto PixelShaderiter : m_pixelShaders)
        {
            PixelShaderiter.second->Initialize(m_d3dDevice.Get());
        }

        //set primitive topology
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        //=============================================================
        //create constant buffer deals with projection matrix
        D3D11_BUFFER_DESC b1 =
        {
            .ByteWidth = sizeof(CBChangeOnResize),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0,
            .StructureByteStride = 0
        };

        hr = m_d3dDevice->CreateBuffer(&b1, nullptr, m_cbChangeOnResize.GetAddressOf());
        if (FAILED(hr))
            return hr;

        //create renderer constant buffer and update
        CBChangeOnResize cb1;
        cb1.Projection = XMMatrixTranspose(m_projection);
        m_immediateContext->UpdateSubresource(m_cbChangeOnResize.Get(), 0, nullptr, &cb1, 0, 0);

        //initialize camera(create constant buffer - view matrix)
        m_camera.Initialize(m_d3dDevice.Get());

        //===============================================================
        //create constant buffer deals with lights
        D3D11_BUFFER_DESC b3 =
        {
            .ByteWidth = sizeof(CBLights),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0,
            .StructureByteStride = 0
        };
        hr = m_d3dDevice->CreateBuffer(&b3, nullptr, m_cbLights.GetAddressOf());
        if (FAILED(hr))
            return hr;

        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddRenderable

      Summary:  Add a renderable object

      Args:     PCWSTR pszRenderableName
                  Key of the renderable object
                const std::shared_ptr<Renderable>& renderable
                  Shared pointer to the renderable object

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddRenderable definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddRenderable(_In_ PCWSTR pszRenderableName, _In_ const std::shared_ptr<Renderable>& renderable)
    {
        if (m_renderables.contains(pszRenderableName))
            return E_FAIL;
        else
        {
            m_renderables[pszRenderableName] = renderable;
            return S_OK;
        }
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPointLight

      Summary:  Add a point light

      Args:     size_t index
                  Index of the point light
                const std::shared_ptr<PointLight>& pointLight
                  Shared pointer to the point light object

      Modifies: [m_aPointLights].

      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddPointLight definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddPointLight(_In_ size_t index, _In_ const std::shared_ptr<PointLight>& pPointLight)
    {
        if (index >= NUM_LIGHTS)
            return E_FAIL;
        else
        {
            m_aPointLights[index] = pPointLight;
            return S_OK;
        }

    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddVertexShader

      Summary:  Add the vertex shader into the renderer

      Args:     PCWSTR pszVertexShaderName
                  Key of the vertex shader
                const std::shared_ptr<VertexShader>&
                  Vertex shader to add

      Modifies: [m_vertexShaders].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddVertexShader definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddVertexShader(_In_ PCWSTR pszVertexShaderName, _In_ const std::shared_ptr<VertexShader>& vertexShader)
    {
        if (m_vertexShaders.contains(pszVertexShaderName))
            return E_FAIL;
        else
        {
            m_vertexShaders[pszVertexShaderName] = vertexShader;
            return S_OK;
        }
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPixelShader

      Summary:  Add the pixel shader into the renderer

      Args:     PCWSTR pszPixelShaderName
                  Key of the pixel shader
                const std::shared_ptr<PixelShader>&
                  Pixel shader to add

      Modifies: [m_pixelShaders].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddPixelShader definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddPixelShader(_In_ PCWSTR pszPixelShaderName, _In_ const std::shared_ptr<PixelShader>& pixelShader)
    {
        if (m_pixelShaders.contains(pszPixelShaderName))
            return E_FAIL;
        else
        {
            m_pixelShaders[pszPixelShaderName] = pixelShader;
            return S_OK;
        }
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::HandleInput

      Summary:  Add the pixel shader into the renderer and initialize it

      Args:     const DirectionsInput& directions
                  Data structure containing keyboard input data
                const MouseRelativeMovement& mouseRelativeMovement
                  Data structure containing mouse relative input data

      Modifies: [m_camera].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::HandleInput definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::HandleInput(_In_ const DirectionsInput& directions, _In_ const MouseRelativeMovement& mouseRelativeMovement, _In_ FLOAT deltaTime)
    {
        m_camera.HandleInput(directions, mouseRelativeMovement, deltaTime);
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Update

      Summary:  Update the renderables each frame

      Args:     FLOAT deltaTime
                  Time difference of a frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Update definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::Update(_In_ FLOAT deltaTime)
    {
        for (auto Renderableiter : m_renderables)
        {
            Renderableiter.second->Update(deltaTime);
        }

        m_camera.Update(deltaTime);

        for (auto Lightiter : m_aPointLights)
        {
            Lightiter->Update(deltaTime);
        }
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
        //clear the back buffer
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::MidnightBlue);

        //clear the depth buffer to 1.0
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        //create camera constant buffer and update
        CBChangeOnCameraMovement cb0;
        cb0.View = XMMatrixTranspose(m_camera.GetView());               
        XMStoreFloat4(&cb0.CameraPosition, m_camera.GetEye());
        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &cb0, 0, 0);

        CBLights cb3;
        for (UINT i = 0; i < NUM_LIGHTS; ++i)
        {
            cb3.LightColors[i] = m_aPointLights[i]->GetColor();
            cb3.LightPositions[i] = m_aPointLights[i]->GetPosition();
        }

        m_immediateContext->UpdateSubresource(m_cbLights.Get(), 0, nullptr, &cb3, 0, 0);

        //create light constant buffer
        for (auto Renderableiter : m_renderables)
        {
            //set vertex buffer
            UINT stride = sizeof(SimpleVertex);
            UINT offset = 0;
            m_immediateContext->IASetVertexBuffers(0, 1, Renderableiter.second->GetVertexBuffer().GetAddressOf(), &stride, &offset);

            //set index buffer
            m_immediateContext->IASetIndexBuffer(Renderableiter.second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

            //set input layout
            m_immediateContext->IASetInputLayout(Renderableiter.second->GetVertexLayout().Get());

            //create renderable constant buffer and update
            CBChangesEveryFrame cb2 =
            {
                .World = XMMatrixTranspose(Renderableiter.second->GetWorldMatrix()),
                .OutputColor = Renderableiter.second->GetOutputColor()
            };

            m_immediateContext->UpdateSubresource(Renderableiter.second->GetConstantBuffer().Get(), 0, nullptr, &cb2, 0, 0);


            //set shader
            m_immediateContext->VSSetShader(Renderableiter.second->GetVertexShader().Get(), nullptr, 0);

            //set constant buffer
            m_immediateContext->VSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(1, 1, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2, 1, Renderableiter.second->GetConstantBuffer().GetAddressOf());

            //Texture resource view of the renderable must be set into the pixel shader
            m_immediateContext->PSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2, 1, Renderableiter.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());

            m_immediateContext->PSSetShader(Renderableiter.second->GetPixelShader().Get(), nullptr, 0);

            //<set shader resource and samplers>

            //use texture 
            if (Renderableiter.second->HasTexture())
            {
                m_immediateContext->PSSetShaderResources(0, 1, Renderableiter.second->GetTextureResourceView().GetAddressOf());

                //Sampler state of the renderable must be set into the pixel shader
                m_immediateContext->PSSetSamplers(0, 1, Renderableiter.second->GetSamplerState().GetAddressOf());
            }
            //render the triangles
            m_immediateContext->DrawIndexed(Renderableiter.second->GetNumIndices(), 0, 0);
        }
        //Present the information rendered to the back buffer to the front buffer
        m_swapChain->Present(0, 0);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetVertexShaderOfRenderable

      Summary:  Sets the vertex shader for a renderable

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszVertexShaderName
                  Key of the vertex shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetVertexShaderOfRenderable definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetVertexShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszVertexShaderName)
    {
        if (m_renderables.contains(pszRenderableName))
        {
            if (m_vertexShaders.contains(pszVertexShaderName))
            {
                m_renderables[pszRenderableName]->SetVertexShader(m_vertexShaders[pszVertexShaderName]);
                return S_OK;
            }
        }
        return E_FAIL;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetPixelShaderOfRenderable

      Summary:  Sets the pixel shader for a renderable

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszPixelShaderName
                  Key of the pixel shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetPixelShaderOfRenderable definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetPixelShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszPixelShaderName)
    {
        if (m_renderables.contains(pszRenderableName))
        {
            if (m_pixelShaders.contains(pszPixelShaderName))
            {
                m_renderables[pszRenderableName]->SetPixelShader(m_pixelShaders[pszPixelShaderName]);
                return S_OK;
            }
        }
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetDriverType

      Summary:  Returns the Direct3D driver type

      Returns:  D3D_DRIVER_TYPE
                  The Direct3D driver type used
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::GetDriverType definition (remove the comment)
    --------------------------------------------------------------------*/
    D3D_DRIVER_TYPE Renderer::GetDriverType() const
    {
        return m_driverType;
    }
}