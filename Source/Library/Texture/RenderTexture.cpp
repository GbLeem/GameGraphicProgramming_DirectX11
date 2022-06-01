#include "RenderTexture.h"

namespace library
{
	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   RenderTexture::RenderTexture

	  Summary:  Constructor

	  Modifies: [m_uWidth, m_uHeight, m_texture2D, m_renderTargetView,
				 m_shaderResourceView, m_samplerClamp].
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	/*--------------------------------------------------------------------
	  TODO: RenderTexture::RenderTexture definition (remove the comment)
	--------------------------------------------------------------------*/
	RenderTexture::RenderTexture(_In_ UINT uWidth, _In_ UINT uHeight)
		:m_uWidth(uWidth)
		,m_uHeight(uHeight)
		,m_texture2D(nullptr)
		,m_renderTargetView(nullptr)
		,m_shaderResourceView(nullptr)
		,m_samplerClamp(nullptr)
	{
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   RenderTexture::Initialize

	  Summary:  Initialize

	  Args:     ID3D11Device* pDevice
				ID3D11DeviceContext* pImmediateContext

	  Modifies: [m_texture2D, m_renderTargetView, m_shaderResourceView,
				 m_samplerClamp].

	  Returns:  HRESULT
				  Status code
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	/*--------------------------------------------------------------------
	  TODO: RenderTexture::Initialize definition (remove the comment)
	--------------------------------------------------------------------*/
	HRESULT RenderTexture::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
	{
		//create texture2D
		D3D11_TEXTURE2D_DESC textureDesc =
		{
			.Width = m_uWidth,
			.Height = m_uHeight,
			.MipLevels = 1,
			.ArraySize = 1,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.SampleDesc = {.Count=1},
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
			.CPUAccessFlags = 0,
			.MiscFlags = 0
		};

		HRESULT hr = pDevice->CreateTexture2D(&textureDesc, NULL, m_texture2D.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"texture2D", L"Error", MB_OK);
			return hr;
		}

		//create rendertarget view
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc =
		{
			.Format = textureDesc.Format,
			.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
			.Texture2D = {.MipSlice = 0 }
		};

		hr = pDevice->CreateRenderTargetView(m_texture2D.Get(), &renderTargetViewDesc, m_renderTargetView.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"rendertarget", L"Error", MB_OK);
			return hr;
		}

		//create shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc =
		{
			.Format = textureDesc.Format,
			.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D,
			.Texture2D = {.MostDetailedMip = 0, .MipLevels = 1}
		};

		hr = pDevice->CreateShaderResourceView(m_texture2D.Get(), &shaderResourceViewDesc, m_shaderResourceView.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"shader resource", L"Error", MB_OK);
			return hr;
		}

		//create sampler state
		D3D11_SAMPLER_DESC sampDesc =
		{
			.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
			.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
			.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
			.ComparisonFunc = D3D11_COMPARISON_ALWAYS,
			.MinLOD = 0,
			.MaxLOD = D3D11_FLOAT32_MAX
		};
		
		hr = pDevice->CreateSamplerState(&sampDesc, m_samplerClamp.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"sampler state", L"Error", MB_OK);
			return hr;
		}

		return S_OK;
	}


	ComPtr<ID3D11Texture2D>& RenderTexture::GetTexture2D()
	{
		return m_texture2D;
	}

	ComPtr<ID3D11RenderTargetView>& RenderTexture::GetRenderTargetView()
	{
		return m_renderTargetView;
	}

	ComPtr<ID3D11ShaderResourceView>& RenderTexture::GetShaderResourceView()
	{
		return m_shaderResourceView;
	}

	ComPtr<ID3D11SamplerState>& RenderTexture::GetSamplerState()
	{
		return m_samplerClamp;
	}

}