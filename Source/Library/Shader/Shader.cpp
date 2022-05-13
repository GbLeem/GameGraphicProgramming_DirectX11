#include "Shader.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Shader::Shader

      Summary:  Constructor

      Args:     PCWSTR pszFileName
                  Name of the file that contains the shader code
              PCSTR pszEntryPoint
                  Name of the shader entry point functino where shader
                  execution begins
              PCSTR pszShaderModel
                  Specifies the shader target or set of shader features
                  to compile against

      Modifies: [m_pszFileName, m_pszEntryPoint, m_pszShaderModel].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    Shader::Shader(_In_ PCWSTR pszFileName, _In_ PCSTR pszEntryPoint, _In_ PCSTR pszShaderModel)
        :m_pszFileName(pszFileName)
        ,m_pszEntryPoint(pszEntryPoint)
        ,m_pszShaderModel(pszShaderModel)
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Shader::GetFileName

      Summary:  Returns the shader file name

      Returns:  PCWSTR
                  Shader file name
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    PCWSTR Shader::GetFileName() const
    {
        return m_pszFileName;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Shader::compile

      Summary:  Compiles the given shader file

      Args:     ID3DBlob** ppOutBlob
                  Receives a pointer to the ID3DBlob interface that you
                  can use to access the compiled code

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Shader::compile(_Outptr_ ID3DBlob** ppOutBlob)
    {
        HRESULT hr = S_OK;
        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined (DEBUG) || defined (_DEBUG)
        dwShaderFlags |= D3DCOMPILE_DEBUG;
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif     

        ComPtr<ID3DBlob> pErrorBlob(nullptr);
        hr = D3DCompileFromFile(m_pszFileName, nullptr, nullptr, m_pszEntryPoint, m_pszShaderModel, dwShaderFlags, 0u, ppOutBlob, pErrorBlob.GetAddressOf());

        if (FAILED(hr))
            return hr;

        /*if (pErrorBlob)
        {
            pErrorBlob.Reset();
            return hr;
        }*/

        return S_OK;
    }
}
