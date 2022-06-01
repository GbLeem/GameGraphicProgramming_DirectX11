#include "Renderer/Renderer.h"

namespace library
{

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer

      Summary:  Constructor

      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                  m_immediateContext, m_immediateContext1, m_swapChain,
                  m_swapChain1, m_renderTargetView, m_depthStencil,
                  m_depthStencilView, m_cbChangeOnResize, m_cbShadowMatrix,
                  m_pszMainSceneName, m_camera, m_projection, m_scenes
                  m_invalidTexture, m_shadowMapTexture, m_shadowVertexShader,
                  m_shadowPixelShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Renderer definition (remove the comment)
    --------------------------------------------------------------------*/
    Renderer::Renderer()
        : m_driverType(D3D_DRIVER_TYPE_NULL)
        , m_featureLevel(D3D_FEATURE_LEVEL_11_0)
        , m_d3dDevice()
        , m_d3dDevice1()
        , m_immediateContext()
        , m_immediateContext1()
        , m_swapChain()
        , m_swapChain1()
        , m_renderTargetView()
        , m_depthStencil()
        , m_depthStencilView()
        , m_cbChangeOnResize()
        , m_pszMainSceneName(nullptr)
        , m_padding{ '\0' }
        , m_camera(XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f))
        , m_projection()
        , m_scenes()
        , m_invalidTexture(std::make_shared<Texture>(L"Content/Common/InvalidTexture.png"))
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
                  m_vertexLayout, m_pixelShader, m_vertexBuffer
                  m_cbShadowMatrix].

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

        GetClientRect(hWnd, &rc);
        UINT uWidth = static_cast<UINT>(rc.right - rc.left);
        UINT uHeight = static_cast<UINT>(rc.bottom - rc.top);

        UINT uCreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
        uCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
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
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, uCreateDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, uCreateDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                    D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());
            }

            if (SUCCEEDED(hr))
            {
                break;
            }
        }
        if (FAILED(hr))
        {
            return hr;
        }

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        ComPtr<IDXGIFactory1> dxgiFactory;
        {
            ComPtr<IDXGIDevice> dxgiDevice;
            hr = m_d3dDevice.As(&dxgiDevice);
            if (SUCCEEDED(hr))
            {
                ComPtr<IDXGIAdapter> adapter;
                hr = dxgiDevice->GetAdapter(&adapter);
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
                }
            }
        }
        if (FAILED(hr))
        {
            return hr;
        }

        // Create swap chain
        ComPtr<IDXGIFactory2> dxgiFactory2;
        hr = dxgiFactory.As(&dxgiFactory2);
        if (SUCCEEDED(hr))
        {
            // DirectX 11.1 or later
            hr = m_d3dDevice.As(&m_d3dDevice1);
            if (SUCCEEDED(hr))
            {
                m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd =
            {
                .Width = uWidth,
                .Height = uHeight,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .SampleDesc = {.Count = 1u, .Quality = 0u },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1u
            };

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, m_swapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                hr = m_swapChain1.As(&m_swapChain);
            }
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd =
            {
                .BufferDesc = {.Width = uWidth, .Height = uHeight, .RefreshRate = {.Numerator = 60, .Denominator = 1 }, .Format = DXGI_FORMAT_R8G8B8A8_UNORM },
                .SampleDesc = {.Count = 1, .Quality = 0 },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1u,
                .OutputWindow = hWnd,
                .Windowed = TRUE
            };

            hr = dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

        if (FAILED(hr))
        {
            return hr;
        }

        // Create a render target view
        ComPtr<ID3D11Texture2D> pBackBuffer;
        hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        // Create depth stencil texture
        D3D11_TEXTURE2D_DESC descDepth =
        {
            .Width = uWidth,
            .Height = uHeight,
            .MipLevels = 1u,
            .ArraySize = 1u,
            .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
            .SampleDesc = {.Count = 1u, .Quality = 0u },
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_DEPTH_STENCIL,
            .CPUAccessFlags = 0u,
            .MiscFlags = 0u
        };
        hr = m_d3dDevice->CreateTexture2D(&descDepth, nullptr, m_depthStencil.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV =
        {
            .Format = descDepth.Format,
            .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
            .Texture2D = {.MipSlice = 0 }
        };
        hr = m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &descDSV, m_depthStencilView.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

        // Setup the viewport
        D3D11_VIEWPORT vp =
        {
            .TopLeftX = 0.0f,
            .TopLeftY = 0.0f,
            .Width = static_cast<FLOAT>(uWidth),
            .Height = static_cast<FLOAT>(uHeight),
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f,
        };
        m_immediateContext->RSSetViewports(1, &vp);

        // Set primitive topology
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Create the constant buffers
        D3D11_BUFFER_DESC bd =
        {
            .ByteWidth = sizeof(CBChangeOnResize),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0
        };
        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbChangeOnResize.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        // Initialize the projection matrix
        m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, static_cast<FLOAT>(uWidth) / static_cast<FLOAT>(uHeight), 0.01f, 1000.0f);

        CBChangeOnResize cbChangesOnResize =
        {
            .Projection = XMMatrixTranspose(m_projection)
        };
        m_immediateContext->UpdateSubresource(m_cbChangeOnResize.Get(), 0, nullptr, &cbChangesOnResize, 0, 0);

        bd.ByteWidth = sizeof(CBLights);
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0u;

        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbLights.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        //LAB10
        //create constant buffer for m_cbshadowMatrix
        D3D11_BUFFER_DESC cbShadow =
        {
            .ByteWidth = sizeof(CBShadowMatrix),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0
        };
        hr = m_d3dDevice->CreateBuffer(&cbShadow, nullptr, m_cbShadowMatrix.GetAddressOf());
        if (FAILED(hr))
        {
            MessageBox(nullptr, L"shadow constant buffer", L"Error", MB_OK);
            return hr;
        }

        //Initialize 
        m_camera.Initialize(m_d3dDevice.Get());

        if (!m_scenes.contains(m_pszMainSceneName))
        {
            return E_FAIL;
        }

        hr = m_scenes[m_pszMainSceneName]->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_invalidTexture->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
        if (FAILED(hr))
        {
            return hr;
        }

        //LAB10
        //init m_shadowMapTexture ?
        m_shadowMapTexture = std::make_shared<RenderTexture>(uWidth, uHeight);

        //call init function for shadowmap
        hr = m_shadowMapTexture->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
        if (FAILED(hr))
        {
            MessageBox(nullptr, L"shadow init error", L"Error", MB_OK);
            return hr;
        }

        //call init function for pointlight
        for (UINT i = 0; i < NUM_LIGHTS; ++i)
            m_scenes[m_pszMainSceneName]->GetPointLight(i)->Initialize(uWidth, uHeight);

        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
     Method:   Renderer::AddScene
     Summary:  Add scene to renderer
     Args:     PCWSTR pszSceneName
                 The name of the scene
               const std::shared_ptr<Scene>&
                 The shared pointer to Scene
     Modifies: [m_scenes].
     Returns:  HRESULT
                 Status code
   M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddScene(_In_ PCWSTR pszSceneName, _In_ const std::shared_ptr<Scene>& scene)
    {
        if (m_scenes.contains(pszSceneName))
        {
            return E_FAIL;
        }

        m_scenes[pszSceneName] = scene;

        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetSceneOrNull
      Summary:  Return scene with the given name or null
      Args:     PCWSTR pszSceneName
                  The name of the scene
      Returns:  std::shared_ptr<Scene>
                  The shared pointer to Scene
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    std::shared_ptr<Scene> Renderer::GetSceneOrNull(_In_ PCWSTR pszSceneName)
    {
        if (m_scenes.contains(pszSceneName))
        {
            return m_scenes[pszSceneName];
        }

        return nullptr;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetMainScene
      Summary:  Set the main scene
      Args:     PCWSTR pszSceneName
                  The name of the scene
      Modifies: [m_pszMainSceneName].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetMainScene(_In_ PCWSTR pszSceneName)
    {
        if (!m_scenes.contains(pszSceneName))
        {
            return E_FAIL;
        }

        m_pszMainSceneName = pszSceneName;

        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetShadowMapShaders

      Summary:  Set shaders for the shadow mapping

      Args:     std::shared_ptr<ShadowVertexShader>
                  vertex shader
                std::shared_ptr<PixelShader>
                  pixel shader

      Modifies: [m_shadowVertexShader, m_shadowPixelShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::SetShadowMapShaders(_In_ std::shared_ptr<ShadowVertexShader> vertexShader, _In_ std::shared_ptr<PixelShader> pixelShader)
    {
        m_shadowVertexShader = move(vertexShader);
        m_shadowPixelShader = move(pixelShader);
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::HandleInput
      Summary:  Handle user mouse input
      Args:     DirectionsInput& directions
                MouseRelativeMovement& mouseRelativeMovement
                FLOAT deltaTime
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
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
    void Renderer::Update(_In_ FLOAT deltaTime)
    {
        m_scenes[m_pszMainSceneName]->Update(deltaTime);

        m_camera.Update(deltaTime);
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
        RenderSceneToTexture();

        //clear the back buffer
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::MidnightBlue);

        //clear the depth buffer to 1.0
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        //create camera constant buffer and update
        CBChangeOnCameraMovement cb0;
        cb0.View = XMMatrixTranspose(m_camera.GetView());
        XMStoreFloat4(&cb0.CameraPosition, m_camera.GetEye());
        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &cb0, 0, 0);

        //change CBLights
        CBLights cb3 = {};
        for (UINT i = 0; i < NUM_LIGHTS; ++i)
        {
            cb3.LightPositions[i] = m_scenes[m_pszMainSceneName]->GetPointLight(i)->GetPosition();
            cb3.LightColors[i] = m_scenes[m_pszMainSceneName]->GetPointLight(i)->GetColor();
            cb3.LightViews[i] = m_scenes[m_pszMainSceneName]->GetPointLight(i)->GetViewMatrix();
            cb3.LightProjections[i] = m_scenes[m_pszMainSceneName]->GetPointLight(i)->GetProjectionMatrix();
        }
        m_immediateContext->UpdateSubresource(m_cbLights.Get(), 0, nullptr, &cb3, 0, 0);

        //render renderables
        for (auto Renderableiter : m_scenes[m_pszMainSceneName]->GetRenderables())
        {
            //set vertex buffer
            UINT stride = sizeof(SimpleVertex);
            UINT Nstride = sizeof(NormalData);
            
            UINT offset = 0;

            m_immediateContext->IASetVertexBuffers(0u, 1u, Renderableiter.second->GetVertexBuffer().GetAddressOf(), &stride, &offset);
            m_immediateContext->IASetVertexBuffers(1u, 1u, Renderableiter.second->GetNormalBuffer().GetAddressOf(), &Nstride, &offset);

            //set index buffer
            m_immediateContext->IASetIndexBuffer(Renderableiter.second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

            //set input layout
            m_immediateContext->IASetInputLayout(Renderableiter.second->GetVertexLayout().Get());

            //set primitive topology
            m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            //create renderable constant buffer and update
            CBChangesEveryFrame cb2 =
            {
                .World = XMMatrixTranspose(Renderableiter.second->GetWorldMatrix()),
                .OutputColor = Renderableiter.second->GetOutputColor(),
                .HasNormalMap = Renderableiter.second->HasNormalMap()
            };

            m_immediateContext->UpdateSubresource(Renderableiter.second->GetConstantBuffer().Get(), 0, nullptr, &cb2, 0, 0);

            //set vertex shader + bind cblight constant buffer
            m_immediateContext->VSSetShader(Renderableiter.second->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->VSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2u, 1u, Renderableiter.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

            //set pixel shader
            m_immediateContext->PSSetShader(Renderableiter.second->GetPixelShader().Get(), nullptr, 0);
            m_immediateContext->PSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2u, 1u, Renderableiter.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());


            //<set shader resource and samplers>
            if (Renderableiter.second->HasTexture())
            {
                for (UINT i = 0; i < Renderableiter.second->GetNumMeshes(); ++i)
                {
                    UINT index = Renderableiter.second->GetMesh(i).uMaterialIndex;
                    if (Renderableiter.second->GetMaterial(index)->pDiffuse)
                    {
                       m_immediateContext->PSSetShaderResources(0u, 1u, Renderableiter.second->GetMaterial(index)->pDiffuse->GetTextureResourceView().GetAddressOf());
                        m_immediateContext->PSSetSamplers(0u, 1u, Renderableiter.second->GetMaterial(index)->pDiffuse->GetSamplerState().GetAddressOf());
                    }
                    if (Renderableiter.second->GetMaterial(index)->pNormal)
                    {
                        m_immediateContext->PSSetShaderResources(1u, 1u, Renderableiter.second->GetMaterial(index)->pNormal->GetTextureResourceView().GetAddressOf());
                        m_immediateContext->PSSetSamplers(1u, 1u, Renderableiter.second->GetMaterial(index)->pNormal->GetSamplerState().GetAddressOf());
                    }
                    //bind texture and sampler state of the shadow map 
                    m_immediateContext->PSSetShaderResources(2u, 1u, m_shadowMapTexture->GetShaderResourceView().GetAddressOf());
                    m_immediateContext->PSSetSamplers(2u, 1u, m_shadowMapTexture->GetSamplerState().GetAddressOf());

                    m_immediateContext->DrawIndexed(Renderableiter.second->GetMesh(i).uNumIndices, Renderableiter.second->GetMesh(i).uBaseIndex, Renderableiter.second->GetMesh(i).uBaseVertex);
                }
            }
            else
                m_immediateContext->DrawIndexed(Renderableiter.second->GetNumIndices(), 0, 0);
            
        }

        //instance data render
        for (auto iter : m_scenes[m_pszMainSceneName]->GetVoxels())
        {
            UINT stride = sizeof(SimpleVertex);
            UINT Nstride = sizeof(NormalData);
            UINT Istride = sizeof(InstanceData);
            UINT offset = 0u;

            //set the buffer
            m_immediateContext->IASetVertexBuffers(0u, 1u, iter->GetVertexBuffer().GetAddressOf(), &stride, &offset);
            m_immediateContext->IASetVertexBuffers(1u, 1u, iter->GetNormalBuffer().GetAddressOf(), &Nstride, &offset);
            m_immediateContext->IASetVertexBuffers(2u, 1u, iter->GetInstanceBuffer().GetAddressOf(), &Istride, &offset);

            m_immediateContext->IASetIndexBuffer(iter->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0u);

            //set the input layout
            m_immediateContext->IASetInputLayout(iter->GetVertexLayout().Get());

            //set primitive topology
            m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            //update the constant buffer
            CBChangesEveryFrame cb =
            {
                .World = XMMatrixTranspose(iter->GetWorldMatrix()),
                .OutputColor = iter->GetOutputColor(),
                .HasNormalMap = iter->HasNormalMap()
            };
            m_immediateContext->UpdateSubresource(iter->GetConstantBuffer().Get(), 0u, nullptr, &cb, 0u, 0u);

            //Set the vertex shader and constant buffers
            m_immediateContext->VSSetShader(iter->GetVertexShader().Get(), nullptr, 0u);
            m_immediateContext->VSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2u, 1u, iter->GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

            // Set the pixel shader and constant buffers
            m_immediateContext->PSSetShader(iter->GetPixelShader().Get(), nullptr, 0u);
            m_immediateContext->PSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2u, 1u, iter->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

            //<set shader resource and samplers>
            if (iter->HasTexture())
            {
                for (UINT i = 0; i < iter->GetNumMeshes(); ++i)
                {
                    UINT index = iter->GetMesh(i).uMaterialIndex;
                    if (iter->GetMaterial(index)->pDiffuse)
                    {
                        m_immediateContext->PSSetShaderResources(0u, 1u, iter->GetMaterial(index)->pDiffuse->GetTextureResourceView().GetAddressOf());
                        m_immediateContext->PSSetSamplers(0u, 1u, iter->GetMaterial(index)->pDiffuse->GetSamplerState().GetAddressOf());
                    }
                    if (iter->GetMaterial(index)->pNormal)
                    {
                        m_immediateContext->PSSetShaderResources(1u, 1u, iter->GetMaterial(index)->pNormal->GetTextureResourceView().GetAddressOf());
                        m_immediateContext->PSSetSamplers(1u, 1u, iter->GetMaterial(index)->pNormal->GetSamplerState().GetAddressOf());
                    }
                    m_immediateContext->PSSetShaderResources(2u, 1u, m_shadowMapTexture->GetShaderResourceView().GetAddressOf());
                    m_immediateContext->PSSetSamplers(2u, 1u, m_shadowMapTexture->GetSamplerState().GetAddressOf());

                    m_immediateContext->DrawIndexedInstanced(iter->GetNumIndices(), iter->GetNumInstances(), 0u, 0u, 0u);
                }
            }
            else
                m_immediateContext->DrawIndexedInstanced(iter->GetNumIndices(), iter->GetNumInstances(), 0u, 0u, 0u);
        }

        //model render
        for (auto Modeliter : m_scenes[m_pszMainSceneName]->GetModels())
        {
            //set vertex buffer
            UINT stride = sizeof(SimpleVertex);
            UINT Nstride = sizeof(NormalData);
            UINT Astride = sizeof(AnimationData);
            UINT aOffsets = 0u;

            m_immediateContext->IASetVertexBuffers(0u, 1u, Modeliter.second->GetVertexBuffer().GetAddressOf(), &stride, &aOffsets);
            m_immediateContext->IASetVertexBuffers(1u, 1u, Modeliter.second->GetNormalBuffer().GetAddressOf(), &Nstride, &aOffsets);
            m_immediateContext->IASetVertexBuffers(2u, 1u, Modeliter.second->GetAnimationBuffer().GetAddressOf(), &Astride, &aOffsets);

            //set index buffer
            m_immediateContext->IASetIndexBuffer(Modeliter.second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

            //set input layout
            m_immediateContext->IASetInputLayout(Modeliter.second->GetVertexLayout().Get());

            //set primitive topology
            m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            //create renderable constant buffer and update
            CBChangesEveryFrame cb2 =
            {
                .World = XMMatrixTranspose(Modeliter.second->GetWorldMatrix()),
                .OutputColor = Modeliter.second->GetOutputColor(),
                .HasNormalMap = Modeliter.second->HasNormalMap()
            };
            m_immediateContext->UpdateSubresource(Modeliter.second->GetConstantBuffer().Get(), 0u, nullptr, &cb2, 0u, 0u);

            //Additional constant buffer CBSkinning
            CBSkinning cb4 = {};
            for (UINT i = 0u; i < Modeliter.second->GetBoneTransforms().size(); ++i)
            {
                cb4.BoneTransforms[i] = XMMatrixTranspose(Modeliter.second->GetBoneTransforms()[i]);
            }

            m_immediateContext->UpdateSubresource(Modeliter.second->GetSkinningConstantBuffer().Get(), 0u, nullptr, &cb4, 0u, 0u);

            //set shader & set constant buffer
            m_immediateContext->VSSetShader(Modeliter.second->GetVertexShader().Get(), nullptr, 0u);
            m_immediateContext->VSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2u, 1u, Modeliter.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());
 
            //set ps constant buffer
            m_immediateContext->PSSetShader(Modeliter.second->GetPixelShader().Get(), nullptr, 0);
            m_immediateContext->PSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2u, 1u, Modeliter.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

            //<set shader resource and samplers>
            if (Modeliter.second->HasTexture())
            {
                for (UINT i = 0; i < Modeliter.second->GetNumMeshes(); ++i)
                {
                    UINT index = Modeliter.second->GetMesh(i).uMaterialIndex;
                    if (Modeliter.second->GetMaterial(index)->pDiffuse)
                    {
                        m_immediateContext->PSSetShaderResources(0u, 1u, Modeliter.second->GetMaterial(index)->pDiffuse->GetTextureResourceView().GetAddressOf());
                        m_immediateContext->PSSetSamplers(0u, 1u, Modeliter.second->GetMaterial(index)->pDiffuse->GetSamplerState().GetAddressOf());
                    }
                    if (Modeliter.second->GetMaterial(index)->pNormal)
                    {
                        m_immediateContext->PSSetShaderResources(1u, 1u, Modeliter.second->GetMaterial(index)->pNormal->GetTextureResourceView().GetAddressOf());
                        m_immediateContext->PSSetSamplers(1u, 1u, Modeliter.second->GetMaterial(index)->pNormal->GetSamplerState().GetAddressOf());
                    }
                    m_immediateContext->PSSetShaderResources(2u, 1u, m_shadowMapTexture->GetShaderResourceView().GetAddressOf());
                    m_immediateContext->PSSetSamplers(2u, 1u, m_shadowMapTexture->GetSamplerState().GetAddressOf());

                    m_immediateContext->DrawIndexed(Modeliter.second->GetMesh(i).uNumIndices, Modeliter.second->GetMesh(i).uBaseIndex, Modeliter.second->GetMesh(i).uBaseVertex);
                }
            }
            else
                m_immediateContext->DrawIndexed(Modeliter.second->GetNumIndices(), 0, 0);
        }

        m_swapChain->Present(0, 0);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::RenderSceneToTexture

      Summary:  Render scene to the texture
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::RenderSceneToTexture definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::RenderSceneToTexture()
    {
        //Unbind current pixel shader resource
        ID3D11ShaderResourceView* const pSRV[2] = {NULL, NULL};
        m_immediateContext->PSSetShaderResources(0, 2, pSRV);
        m_immediateContext->PSSetShaderResources(2, 1, pSRV);

        std::shared_ptr<RenderTexture> shadowMapTexture;
        //change render target to shadow map texture
        m_immediateContext->OMSetRenderTargets(1, m_shadowMapTexture->GetRenderTargetView().GetAddressOf(), m_depthStencilView.Get());

        //clear render target view with white
        m_immediateContext->ClearRenderTargetView(m_shadowMapTexture->GetRenderTargetView().Get(), Colors::White);
        
        //clear depth stencil view
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);

        //render renderables/voxels/models with shadow map shader
        for (auto renderable : m_scenes[m_pszMainSceneName]->GetRenderables())
        {
            UINT uStride = sizeof(SimpleVertex);
            UINT uOffset = 0;
            //bind buffers
            m_immediateContext->IASetVertexBuffers(0u, 1u, renderable.second->GetVertexBuffer().GetAddressOf(),&uStride, &uOffset);
            m_immediateContext->IASetIndexBuffer(renderable.second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(/*m_shadowVertexShader*/renderable.second->GetVertexLayout().Get());

            //bind vertex shader and pixel shader
            m_immediateContext->VSSetShader(/*m_shadowVertexShader*/renderable.second->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->PSSetShader(/*m_shadowPixelShader*/renderable.second->GetPixelShader().Get(), nullptr, 0);

            //update and bind CBShadowMatrix constant buffer
            CBShadowMatrix cb =
            {
                .World = XMMatrixTranspose(renderable.second->GetWorldMatrix()),
                .View = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetViewMatrix()),
                .Projection = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetProjectionMatrix()),
                .IsVoxel = false
            };
            m_immediateContext->UpdateSubresource(/*m_cbShadowMatrix*/renderable.second->GetConstantBuffer().Get(), 0, nullptr, &cb, 0, 0);

            m_immediateContext->VSSetConstantBuffers(0u, 1u, /*m_cbShadowMatrix*/renderable.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(0u, 1u, /*m_cbShadowMatrix*/renderable.second->GetConstantBuffer().GetAddressOf());

            //draw
            m_immediateContext->DrawIndexed(renderable.second->GetNumIndices(), 0, 0);
        }

        for (auto voxel : m_scenes[m_pszMainSceneName]->GetVoxels())
        {
            UINT uStride = sizeof(SimpleVertex);
            UINT uOffset = 0;
            //bind buffers
            m_immediateContext->IASetVertexBuffers(0u, 1u, voxel->GetVertexBuffer().GetAddressOf(), &uStride, &uOffset);
            m_immediateContext->IASetIndexBuffer(voxel->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(/*m_shadowVertexShader*/voxel->GetVertexLayout().Get());

            //bind vertex shader and pixel shader
            m_immediateContext->VSSetShader(/*m_shadowVertexShader*/voxel->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->PSSetShader(/*m_shadowPixelShader*/voxel->GetPixelShader().Get(), nullptr, 0);

            //update and bind CBShadowMatrix constant buffer
            CBShadowMatrix cb =
            {
                .World = XMMatrixTranspose(voxel->GetWorldMatrix()),
                .View = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetViewMatrix()),
                .Projection = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetProjectionMatrix()),
                .IsVoxel = false
            };
            m_immediateContext->UpdateSubresource(/*m_cbShadowMatrix*/voxel->GetConstantBuffer().Get(), 0, nullptr, &cb, 0, 0);
            
            m_immediateContext->VSSetConstantBuffers(0u, 1u, /*m_cbShadowMatrix*/voxel->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(0u, 1u, /*m_cbShadowMatrix*/voxel->GetConstantBuffer().GetAddressOf());

            //draw
            m_immediateContext->DrawIndexedInstanced(voxel->GetNumIndices(), voxel->GetNumInstances(), 0u, 0u, 0u);
        }

        for (auto model : m_scenes[m_pszMainSceneName]->GetModels())
        {
            UINT uStride = sizeof(SimpleVertex);
            UINT uOffset = 0;
            //bind buffers
            m_immediateContext->IASetVertexBuffers(0u, 1u, model.second->GetVertexBuffer().GetAddressOf(), &uStride, &uOffset);
            m_immediateContext->IASetIndexBuffer(model.second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(/*m_shadowVertexShader*/model.second->GetVertexLayout().Get());

            //bind vertex shader and pixel shader
            m_immediateContext->VSSetShader(m_shadowVertexShader->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->PSSetShader(m_shadowPixelShader->GetPixelShader().Get(), nullptr, 0);

            //update and bind CBShadowMatrix constant buffer
            CBShadowMatrix cb =
            {
                .World = XMMatrixTranspose(model.second->GetWorldMatrix()),
                .View = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetViewMatrix()),
                .Projection = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetProjectionMatrix()),
                .IsVoxel = false
            };
            m_immediateContext->UpdateSubresource(model.second->GetConstantBuffer().Get(), 0, nullptr, &cb, 0, 0);

            m_immediateContext->VSSetConstantBuffers(0u, 1u, /*m_cbShadowMatrix*/model.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(0u, 1u,  /*m_cbShadowMatrix*/model.second->GetConstantBuffer().GetAddressOf());

            //draw
            m_immediateContext->DrawIndexed(model.second->GetNumIndices(), 0, 0);
        }

        //reset the render target to the original back buffer
        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetDriverType
      Summary:  Returns the Direct3D driver type
      Returns:  D3D_DRIVER_TYPE
                  The Direct3D driver type used
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    D3D_DRIVER_TYPE Renderer::GetDriverType() const
    {
        return m_driverType;
    }
}