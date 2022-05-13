#include "Scene/Voxel.h"

#include "Texture/Material.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::Voxel

      Summary:  Constructor

      Args:     const XMFLOAT4& outputColor
                  Color of the voxel
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    Voxel::Voxel(_In_ const XMFLOAT4& outputColor)
        :InstancedRenderable(outputColor)
    {

    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::Voxel

      Summary:  Constructor

      Args:     std::vector<InstanceData>&& aInstanceData
                  Instance data
                const XMFLOAT4& outputColor
                  Color of the voxel
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    Voxel::Voxel(_In_ std::vector<InstanceData>&& aInstanceData, _In_ const XMFLOAT4& outputColor)
        :InstancedRenderable(std::move(aInstanceData), outputColor)
    {

    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::Initialize

      Summary:  Initializes a voxel

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Voxel::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        HRESULT hr = S_OK;

        hr = initialize(pDevice, pImmediateContext);
        if (FAILED(hr))
            return hr;

        hr = initializeInstance(pDevice);
        if (FAILED(hr))
            return hr;

        return hr;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::Update

      Summary:  Updates the voxel every frame

      Args:     FLOAT deltaTime
                  Elapsed time
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Voxel::Update(_In_ FLOAT deltaTime)
    {
        //¸ÂÀ½?
        //UNREFERENCED_PARAMETER(deltaTime);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::GetNumVertices

      Summary:  Returns the number of vertices in the voxel

      Returns:  UINT
                  Number of vertices
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    UINT Voxel::GetNumVertices() const
    {
        return NUM_VERTICES;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::GetNumIndices

      Summary:  Returns the number of indices in the voxel

      Returns:  UINT
                  Number of indices
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    UINT Voxel::GetNumIndices() const
    {
        return NUM_INDICES;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::getVertices

      Summary:  Returns the pointer to the vertices data

      Returns:  const library::SimpleVertex*
                  Pointer to the vertices data
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const SimpleVertex* Voxel::getVertices() const
    {
        return VERTICES;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::getIndices

      Summary:  Returns the pointer to the indices data

      Returns:  const WORD*
                  Pointer to the indices data
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const WORD* Voxel::getIndices() const
    {
        return INDICES;
    }
}