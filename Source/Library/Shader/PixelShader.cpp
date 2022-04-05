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
    /*--------------------------------------------------------------------
      TODO: PixelShader::PixelShader definition (remove the comment)
    --------------------------------------------------------------------*/

    PixelShader::PixelShader(PCWSTR pszFileName, PCSTR pszEntryPoint, PCSTR pszShaderModel)
        :m_pixelShader(nullptr)
        /*,m_pszFileName(pszFileName)
        ,m_pszEntryPoint(pszEntryPoint)
        ,m_pszShaderModel(pszShaderModel)*/
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
    /*--------------------------------------------------------------------
      TODO: PixelShader::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT PixelShader::Initialize(ID3D11Device* pDevice)
    {
        //Compile the pixel shader
        ComPtr<ID3DBlob> pPSBlob(nullptr);

        HRESULT hr = compile(pPSBlob.GetAddressOf());

        if (FAILED(hr))
        {
            MessageBox(nullptr,
                L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }

        //Create the pixel shader
        hr = pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, GetPixelShader().GetAddressOf());

        if (FAILED(hr))
            return hr;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   PixelShader::GetPixelShader

      Summary:  Returns the pixel shader

      Returns:  ComPtr<ID3D11PixelShader>&
                  Pixel shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: PixelShader::GetPixelShader definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11PixelShader>& PixelShader::GetPixelShader()
    {
        return m_pixelShader;
    }
}
