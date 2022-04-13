#include "Renderer/Renderable.h"
#include "Texture/DDSTextureLoader.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::Renderable

      Summary:  Constructor

      Args:     const std::filesystem::path& textureFilePath
                  Path to the texture to use

      Modifies: [m_vertexBuffer, m_indexBuffer, m_constantBuffer, 
                 m_textureRV, m_samplerLinear, m_vertexShader, 
                 m_pixelShader, m_textureFilePath, m_world].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::Renderable definition (remove the comment)
    --------------------------------------------------------------------*/
    Renderable::Renderable(_In_ const std::filesystem::path& textureFilePath)
        :m_vertexBuffer(nullptr)
        ,m_indexBuffer(nullptr)
        ,m_constantBuffer(nullptr)
        ,m_textureRV(nullptr)
        ,m_samplerLinear(nullptr)
        ,m_vertexShader(nullptr)
        ,m_pixelShader(nullptr)
        ,m_textureFilePath(L"seafloor.dds")
        ,m_world()
    {

    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::initialize

      Summary:  Initializes the buffers, texture, and the world matrix

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers

      Modifies: [m_vertexBuffer, m_indexBuffer, m_constantBuffer, 
                 m_textureRV, m_samplerLinear, m_world].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderable::initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        HRESULT hr = S_OK;

        //Create a Vertex Buffer
        D3D11_BUFFER_DESC bd =
        {
            .ByteWidth = sizeof(SimpleVertex) * GetNumVertices(),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0
        };

        D3D11_SUBRESOURCE_DATA initData =
        {
            .pSysMem = getVertices(),
            .SysMemPitch = 0,
            .SysMemSlicePitch = 0
        };

        hr = pDevice->CreateBuffer(&bd, &initData, m_vertexBuffer.GetAddressOf());

        if (FAILED(hr))
            return hr;

        //Create an Index Buffer
        D3D11_BUFFER_DESC bd2 =
        {
            .ByteWidth = sizeof(WORD) * GetNumIndices(),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0
        };

        D3D11_SUBRESOURCE_DATA initData2 =
        {
            .pSysMem = getIndices(),
            .SysMemPitch = 0,
            .SysMemSlicePitch = 0,
        };

        hr = pDevice->CreateBuffer(&bd2, &initData2, &m_indexBuffer);

        if (FAILED(hr))
            return hr;

        //Create constant Buffer deals with world matrix
        D3D11_BUFFER_DESC bd3 =
        {
            .ByteWidth = sizeof(CBChangesEveryFrame),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0,
            .StructureByteStride = 0
        };

        hr = pDevice->CreateBuffer(&bd3, nullptr, m_constantBuffer.GetAddressOf());
        if (FAILED(hr))
            return hr;

        // Initialize the world matrix - world matrix is simply an identity matrix
        m_world = XMMatrixIdentity();

        //Load the Texture
        hr = CreateDDSTextureFromFile(pDevice, m_textureFilePath.filename().wstring().c_str(), nullptr, m_textureRV.GetAddressOf());
        if (FAILED(hr))
            return hr;

        //create the simple state
        D3D11_SAMPLER_DESC sampDesc =
        {
            .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
            .ComparisonFunc = D3D11_COMPARISON_NEVER,
            .MinLOD = 0,
            .MaxLOD = D3D11_FLOAT32_MAX
        };
        
        hr = pDevice->CreateSamplerState(&sampDesc, &m_samplerLinear);
        if (FAILED(hr))
            return hr;
           
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::SetVertexShader

      Summary:  Sets the vertex shader to be used for this renderable
                object

      Args:     const std::shared_ptr<VertexShader>& vertexShader
                  Vertex shader to set to

      Modifies: [m_vertexShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::SetVertexShader definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderable::SetVertexShader(_In_ const std::shared_ptr<VertexShader>& vertexShader)
    {
        m_vertexShader = vertexShader;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::SetPixelShader

      Summary:  Sets the pixel shader to be used for this renderable
                object

      Args:     const std::shared_ptr<PixelShader>& pixelShader
                  Pixel shader to set to

      Modifies: [m_pixelShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::SetPixelShader definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderable::SetPixelShader(_In_ const std::shared_ptr<PixelShader>& pixelShader)
    {
        m_pixelShader = pixelShader;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexShader

      Summary:  Returns the vertex shader

      Returns:  ComPtr<ID3D11VertexShader>&
                  Vertex shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetVertexShader definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11VertexShader>& Renderable::GetVertexShader()
    {
        return m_vertexShader->GetVertexShader();
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetPixelShader

      Summary:  Returns the vertex shader

      Returns:  ComPtr<ID3D11PixelShader>&
                  Pixel shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetPixelShader definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11PixelShader>& Renderable::GetPixelShader()
    {
        return m_pixelShader->GetPixelShader();
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexLayout

      Summary:  Returns the vertex input layout

      Returns:  ComPtr<ID3D11InputLayout>&
                  Vertex input layout
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetVertexLayout definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11InputLayout>& Renderable::GetVertexLayout()
    {
        return m_vertexShader->GetVertexLayout();
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexBuffer

      Summary:  Returns the vertex buffer

      Returns:  ComPtr<ID3D11Buffer>&
                  Vertex buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetVertexBuffer definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11Buffer>& Renderable::GetVertexBuffer()
    {
        return m_vertexBuffer;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetIndexBuffer

      Summary:  Returns the index buffer

      Returns:  ComPtr<ID3D11Buffer>&
                  Index buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetIndexBuffer definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11Buffer>& Renderable::GetIndexBuffer()
    {
        return m_indexBuffer;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetConstantBuffer

      Summary:  Returns the constant buffer

      Returns:  ComPtr<ID3D11Buffer>&
                  Constant buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetConstantBuffer definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11Buffer>& Renderable::GetConstantBuffer()
    {
        return m_constantBuffer;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetWorldMatrix

      Summary:  Returns the world matrix

      Returns:  const XMMATRIX&
                  World matrix
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetWorldMatrix definition (remove the comment)
    --------------------------------------------------------------------*/
    const XMMATRIX& Renderable::GetWorldMatrix() const
    {
        return m_world;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetTextureResourceView

      Summary:  Returns the texture resource view

      Returns:  ComPtr<ID3D11ShaderResourceView>&
                  The texture resource view
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetTextureResourceView definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11ShaderResourceView>& Renderable::GetTextureResourceView()
    {
        return m_textureRV;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetSamplerState

      Summary:  Returns the sampler state

      Returns:  ComPtr<ID3D11SamplerState>&
                  The sampler state
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetSamplerState definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11SamplerState>& Renderable::GetSamplerState()
    {
        return m_samplerLinear;
    }
}
