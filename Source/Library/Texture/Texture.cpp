#include "Texture.h"

#include "Texture/WICTextureLoader.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::Texture

      Summary:  Constructor

      Args:     const std::filesystem::path& textureFilePath
                  Path to the texture to use

      Modifies: [m_filePath, m_textureRV, m_samplerLinear].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    Texture::Texture(_In_ const std::filesystem::path& filePath)
        :m_filePath(filePath)
        ,m_textureRV()
        ,m_samplerLinear()
    {

    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::Initialize

      Summary:  Initializes the texture

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers

      Modifies: [m_textureRV, m_samplerLinear].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Texture::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        HRESULT hr = CreateWICTextureFromFile(pDevice,pImmediateContext, m_filePath.c_str(), nullptr, m_textureRV.GetAddressOf());
        if (FAILED(hr))
        {
            MessageBox(nullptr, L"no texture", L"Error", MB_OK);
            return hr;
        }

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

        hr = pDevice->CreateSamplerState(&sampDesc, m_samplerLinear.GetAddressOf());
        if (FAILED(hr))
            return hr;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::GetTextureResourceView

      Summary:  Constructor

      Returns:  ComPtr<ID3D11ShaderResourceView>&
                  Shader resource view
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11ShaderResourceView>& Texture::GetTextureResourceView()
    {
        return m_textureRV;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::GetSamplerState

      Summary:  Constructor

      Returns:  ComPtr<ID3D11SamplerState>&
                  Sampler state
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11SamplerState>& Texture::GetSamplerState()
    {
        return m_samplerLinear;
    }
}