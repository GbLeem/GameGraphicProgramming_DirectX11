#include "Cube/BaseCube.h"

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
HRESULT BaseCube::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
{
    return initialize(pDevice, pImmediateContext);
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   BaseCube::GetNumVertices

  Summary:  Returns the number of vertices in the cube
  
  Returns:  UINT
              Number of vertices
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
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
UINT BaseCube::GetNumIndices() const
{
    return NUM_INDICES;
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   BaseCube::getVertices

  Summary:  Returns the pointer to the vertices data
  
  Returns:  const library::SimpleVertex*
              Pointer to the vertices data
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
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
const WORD* BaseCube::getIndices() const
{
    return INDICES;
}

