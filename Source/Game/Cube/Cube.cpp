#include "Cube/Cube.h"

Cube::Cube(_In_ const XMFLOAT4& outputColor)
    : BaseCube(outputColor)
{
}

HRESULT Cube::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
{
    BasicMeshEntry basicMeshEntry;
    basicMeshEntry.uNumIndices = NUM_INDICES;

    m_aMeshes.push_back(basicMeshEntry);

    if (HasTexture())
    {
        SetMaterialOfMesh(0, 0);
    }

    return initialize(pDevice, pImmediateContext);
}

void Cube::Update(_In_ FLOAT deltaTime)
{
    // Does nothing
}