#include "Cube/BaseCube.h"

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   BaseCube::BaseCube

  Summary:  Constructor

  Args:     const std::filesystem::path& textureFilePath
              Path to the texture to use
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: BaseCube::BaseCube definition (remove the comment)
--------------------------------------------------------------------*/
BaseCube::BaseCube(const std::filesystem::path& textureFilePath)
    :Renderable(textureFilePath)
{
    
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   BaseCube::Initialize

  Summary:  Initializes a basic cube

  Args:     ID3D11Device* pDevice
              The Direct3D device to create the buffers
            ID3D11DeviceContext* pImmediateContext
              The Direct3D context to set buffers

  Returns:  HRESULT
              Status code
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: BaseCube::Initialize definition (remove the comment)
--------------------------------------------------------------------*/
HRESULT BaseCube::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext) 
{
    /*library::SimpleVertex vertices[] =
    {
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
    };*/

    ////create the sample state
    //D3D11_SAMPLER_DESC sampDesc =
    //{
    //    .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    //    .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
    //    .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
    //    .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
    //    .ComparisonFunc = D3D11_COMPARISON_NEVER,
    //    .MinLOD = 0,
    //    .MaxLOD = D3D11_FLOAT32_MAX
    //};

    //HRESULT hr = pDevice->CreateSamplerState(&sampDesc, m_samplerLinear.GetAddressOf());
    //if (FAILED(hr))
    //    return hr;

    return initialize(pDevice, pImmediateContext);
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   BaseCube::GetNumVertices

  Summary:  Returns the number of vertices in the cube
  
  Returns:  UINT
              Number of vertices
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: BaseCube::GetNumVertices definition (remove the comment)
--------------------------------------------------------------------*/
UINT BaseCube::GetNumVertices() const
{
    return NUM_VERTICES;
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   BaseCube::GetNumIndices

  Summary:  Returns the number of indices in the cube
  
  Returns:  UINT
              Number of indices
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: BaseCube::GetNumIndices definition (remove the comment)
--------------------------------------------------------------------*/
UINT BaseCube::GetNumIndices() const
{
    return NUM_VERTICES;
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   BaseCube::getVertices

  Summary:  Returns the pointer to the vertices data
  
  Returns:  const library::SimpleVertex*
              Pointer to the vertices data
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: BaseCube::getVertices definition (remove the comment)
--------------------------------------------------------------------*/
const library::SimpleVertex* BaseCube::getVertices() const
{
    return VERTICES;
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   BaseCube::getIndices

  Summary:  Returns the pointer to the indices data
  
  Returns:  const WORD*
              Pointer to the indices data
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: BaseCube::getIndices definition (remove the comment)
--------------------------------------------------------------------*/
const WORD* BaseCube::getIndices() const
{
    return INDICES;
}

