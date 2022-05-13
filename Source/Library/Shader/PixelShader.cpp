#include "Shader/PixelShader.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PixelShader::PixelShader

      Summary:  Constructor

      Args:     PCWSTR pszFileName
                  Name of the file that contains the shader code
                PCSTR pszEntryPoint
                  Name of the shader entry point functino where shader
                  execution begins
                PCSTR pszShaderModel
                  Specifies the shader target or set of shader features
                  to compile against

      Modifies: [m_pixelShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    PixelShader::PixelShader(_In_ PCWSTR pszFileName, _In_ PCSTR pszEntryPoint, _In_ PCSTR pszShaderModel)
        :Shader(pszFileName, pszEntryPoint, pszShaderModel)
        ,m_pixelShader()
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PixelShader::Initialize

      Summary:  Initializes the pixel shader

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the pixel shader

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT PixelShader::Initialize(_In_ ID3D11Device* pDevice)
    {
        //Compile the pixel shader
        ComPtr<ID3DBlob> pPSBlob(nullptr);

        HRESULT hr = compile(pPSBlob.GetAddressOf());

        if (FAILED(hr))
        {
            MessageBox(nullptr, L"Pixel Shader compile Error", L"Error", MB_OK);
            return hr;
        }

        //Create the pixel shader
        hr = pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, GetPixelShader().GetAddressOf());

        if (FAILED(hr))
            return hr;

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PixelShader::GetPixelShader

      Summary:  Returns the pixel shader

      Returns:  ComPtr<ID3D11PixelShader>&
                  Pixel shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11PixelShader>& PixelShader::GetPixelShader()
    {
        return m_pixelShader;
    }
}
