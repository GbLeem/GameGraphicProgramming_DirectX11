#include "Shader/VertexShader.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   VertexShader::VertexShader

      Summary:  Constructor

      Args:     PCWSTR pszFileName
                  Name of the file that contains the shader code
                PCSTR pszEntryPoint
                  Name of the shader entry point functino where shader
                  execution begins
                PCSTR pszShaderModel
                  Specifies the shader target or set of shader features
                  to compile against

      Modifies: [m_vertexShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    VertexShader::VertexShader(_In_ PCWSTR pszFileName, _In_ PCSTR pszEntryPoint, _In_ PCSTR pszShaderModel)
        :Shader(pszFileName, pszEntryPoint, pszShaderModel)
        , m_vertexShader()
        , m_vertexLayout()
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   VertexShader::Initialize

      Summary:  Initializes the vertex shader and the input layout

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the vertex shader

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT VertexShader::Initialize(_In_ ID3D11Device* pDevice)
    {
        //Compile the vertex shader 
        ComPtr<ID3DBlob> pVSBlob(nullptr);
        HRESULT hr = compile(pVSBlob.GetAddressOf());

        if (FAILED(hr))
        {
           MessageBox(nullptr, L"Vertex Shader compile Error", L"Error", MB_OK);
            return hr;
        }

        //Create the vertex shader
        hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());

        if (FAILED(hr))
            return hr;

        //Define the input layout (add texture/normal coordinates)
        D3D11_INPUT_ELEMENT_DESC aLayouts[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

        UINT uNumElements = ARRAYSIZE(aLayouts);

        //Create the input layout
        hr = pDevice->CreateInputLayout(aLayouts, uNumElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), m_vertexLayout.GetAddressOf());

        if (FAILED(hr))
            return hr;

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   VertexShader::GetVertexShader

      Summary:  Returns the vertex shader

      Returns:  ComPtr<ID3D11VertexShader>&
                  Vertex shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11VertexShader>& VertexShader::GetVertexShader()
    {
        return m_vertexShader;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   VertexShader::GetVertexLayout

      Summary:  Returns the vertex input layout

      Returns:  ComPtr<ID3D11InputLayout>&
                  Vertex input layout
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11InputLayout>& VertexShader::GetVertexLayout()
    {
        return m_vertexLayout;
    }
}
