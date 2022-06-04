#include "Renderer/Skybox.h"

#include "assimp/Importer.hpp"	// C++ importer interface
#include "assimp/scene.h"		// output data structure
#include "assimp/postprocess.h"	// post processing flags

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Skybox::Skybox

      Summary:  Constructor

      Args:     const std::filesystem::path& cubeMapFilePath
                  Path to the cube map texture to use
                FLOAT scale
                  Scaling factor

      Modifies: [m_cubeMapFileName, m_scale].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Skybox::Skybox definition (remove the comment)
    --------------------------------------------------------------------*/
    Skybox::Skybox(_In_ const std::filesystem::path& cubeMapFilePath, _In_ FLOAT scale)
        :Model(L"Content/Common/Sphere.obj")
        ,m_cubeMapFileName(cubeMapFilePath)
        ,m_scale(scale)
    {

    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Skybox::Initialize

      Summary:  Initializes the skybox and cube map texture

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers

      Modifies: [m_aMeshes, m_aMaterials].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Skybox::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Skybox::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        HRESULT hr = Model::Initialize(pDevice, pImmediateContext);
        if (FAILED(hr))
        {
            MessageBox(nullptr, L"Skybox init error", L"Error", MB_OK);
            return hr;
        }
        //Scale
        Scale(m_scale, m_scale, m_scale);

        //set index
        m_aMeshes[0].uMaterialIndex = 0;
        
        //set and init material's diffuse
        m_aMaterials[0]->pDiffuse = std::make_shared<Texture>(m_cubeMapFileName);
        m_aMaterials[0]->pDiffuse->Initialize(pDevice, pImmediateContext);

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Skybox::GetSkyboxTexture

      Summary:  Returns the cube map texture

      Returns:  const std::shared_ptr<Texture>&
                  Cube map texture object
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Skybox::GetSkyboxTexture definition (remove the comment)
    --------------------------------------------------------------------*/
    const std::shared_ptr<Texture>& Skybox::GetSkyboxTexture() const
    {
        return m_aMaterials[0]->pDiffuse;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Skybox::initSingleMesh

      Summary:  Initialize single mesh from a given assimp mesh

      Args:     UINT uMeshIndex
                  Mesh index
                const aiMesh* pMesh
                  Point to an assimp mesh object
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Skybox::initSingleMesh definition (remove the comment)
    --------------------------------------------------------------------*/
    void Skybox::initSingleMesh(_In_ UINT uMeshIndex, _In_ const aiMesh* pMesh)
    {
        const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

        // Populate the vertex attribute vectors
        for (UINT i = 0u; i < pMesh->mNumVertices; ++i)
        {
            const aiVector3D& pPos = pMesh->mVertices[i];
            const aiVector3D& pNormal = pMesh->mNormals[i];
            const aiVector3D& pTexCoord = pMesh->HasTextureCoords(0u) ? pMesh->mTextureCoords[0u][i] : Zero3D;
            const aiVector3D& tangent = pMesh->HasTangentsAndBitangents() ? pMesh->mTangents[i] : Zero3D;
            const aiVector3D& bitangent = pMesh->HasTangentsAndBitangents() ? pMesh->mBitangents[i] : Zero3D;

            SimpleVertex meshVertex =
            {
                .Position = XMFLOAT3(pPos.x, pPos.y, pPos.z),
                .TexCoord = XMFLOAT2(pTexCoord.x, pTexCoord.y),
                .Normal = XMFLOAT3(pNormal.x, pNormal.y, pNormal.z)
            };
            m_aVertices.push_back(meshVertex);

            NormalData normal =
            {
                .Tangent = XMFLOAT3(tangent.x,tangent.y, tangent.z),
                .Bitangent = XMFLOAT3(bitangent.x, bitangent.y, bitangent.z)
            };
            m_aNormalData.push_back(normal);
        }

        // Populate the index buffer
        for (UINT i = 0u; i < pMesh->mNumFaces; i++)
        {
            const aiFace& Face = pMesh->mFaces[i];
            m_aIndices.push_back(Face.mIndices[2u]);
            m_aIndices.push_back(Face.mIndices[1u]);
            m_aIndices.push_back(Face.mIndices[0u]);
        }

        initMeshBones(uMeshIndex, pMesh);
    }

}