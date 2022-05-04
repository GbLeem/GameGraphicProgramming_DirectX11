#include "Model/Model.h"

#include "assimp/Importer.hpp"	// C++ importer interface
#include "assimp/scene.h"		    // output data structure
#include "assimp/postprocess.h"	// post processing flags

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::Model

      Summary:  Constructor

      Args:     const std::filesystem::path& filePath
                  Path to the model to load

      Modifies: [m_filePath, m_aVertices, m_aIndices].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Model::Model definition (remove the comment)
    --------------------------------------------------------------------*/
    Model::Model(_In_ const std::filesystem::path& filePath)
        :Renderable(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
        ,m_filePath(filePath)
        ,m_aVertices()
        ,m_aIndices()
        ,m_padding()
    {

    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::Initialize

      Summary:  Constructor

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers
      
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Model::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        HRESULT hr = S_OK;

        // Create the buffers for the vertices attributes

        Assimp::Importer importer;

        const aiScene* pScene = importer.ReadFile(
            m_filePath.string().c_str(),
            ASSIMP_LOAD_FLAGS
            );

        if (pScene)
        {
            hr = initFromScene(pDevice, pImmediateContext, pScene, m_filePath);
        }
        else
        {
            hr = E_FAIL;
            OutputDebugString(L"Error parsing ");
            OutputDebugString(m_filePath.c_str());
            OutputDebugString(L": ");
            OutputDebugStringA(importer.GetErrorString());
            OutputDebugString(L"\n");
        }

        return hr;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::Update

      Summary:  Updates the cube every frame

      Args:     FLOAT deltaTime
                  Elapsed time
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Model::Update(_In_ FLOAT deltaTime)
    {
        UNREFERENCED_PARAMETER(deltaTime);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::GetNumVertices

      Summary:  Returns the number of vertices

      Returns:  UINT
                  Number of vertices
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    UINT Model::GetNumVertices() const
    {
        return static_cast<UINT>(m_aVertices.size());
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::GetNumIndices

      Summary:  Returns the number of indices

      Returns:  UINT
                  Number of indices
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    UINT Model::GetNumIndices() const
    {
        return static_cast<UINT>(m_aIndices.size());
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::countVerticesAndIndices

      Summary:  Fill the BasicMeshEntry information

      Args:     UINT& uOutNumVertices
                  Total number of vertices
                UINT& uOutNumIndices
                  Total number of indices
                const aiScene* pScene
                  Pointer to an assimp scene object that contains the 
                  mesh information
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Model::countVerticesAndIndices definition (remove the comment)
    --------------------------------------------------------------------*/
    void Model::countVerticesAndIndices(_Inout_ UINT& uOutNumVertices, _Inout_ UINT& uOutNumIndices, _In_ const aiScene* pScene)
    {
        for (UINT i = 0u; i < pScene->mNumMeshes; ++i)
        {
            m_aMeshes[i].uMaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
            m_aMeshes[i].uNumIndices = pScene->mMeshes[i]->mNumFaces * 3u;
            m_aMeshes[i].uBaseVertex = uOutNumVertices;
            m_aMeshes[i].uBaseIndex = uOutNumIndices;

            uOutNumVertices += pScene->mMeshes[i]->mNumVertices;
            uOutNumIndices += m_aMeshes[i].uNumIndices;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::getVertices

      Summary:  Returns the vertices data

      Returns:  const SimpleVertex*
                  Array of vertices
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const SimpleVertex* Model::getVertices() const
    {
        return m_aVertices.data();
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::getIndices

      Summary:  Returns the indices data

      Returns:  const WORD*
                  Array of indices
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const WORD* Model::getIndices() const
    {
        return m_aIndices.data();
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::getIndices

      Summary:  Initialize all meshes in a given assimp scene

      Args:     const aiScene* pScene
                  Assimp scene
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Model::initAllMeshes(_In_ const aiScene* pScene)
    {
        for (UINT i = 0u; i < m_aMeshes.size(); ++i)
        {
            const aiMesh* pMesh = pScene->mMeshes[i];
            initSingleMesh(pMesh);
        }
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::initFromScene

      Summary:  Initialize all meshes in a given assimp scene

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers
                const aiScene* pScene
                  Assimp scene
                const std::filesystem::path& filePath
                  Path to the model
      
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Model::initFromScene definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Model::initFromScene(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext, _In_ const aiScene* pScene, _In_ const std::filesystem::path& filePath)
    {
        HRESULT hr = S_OK;

        m_aMeshes.resize(pScene->mNumMeshes);
        m_aMaterials.resize(pScene->mNumMaterials);

        UINT NumVertices = 0;
        UINT NumIndices = 0;

        countVerticesAndIndices(NumVertices, NumIndices, pScene);
        reserveSpace(NumVertices, NumIndices);
        initAllMeshes(pScene);
        initMaterials(pDevice, pImmediateContext, pScene, filePath);
        initialize(pDevice, pImmediateContext);
        
        return hr;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::initMaterials

      Summary:  Initialize all materials in a given assimp scene

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers
                const aiScene* pScene
                  Assimp scene
                const std::filesystem::path& filePath
                  Path to the model
      
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Model::initMaterials(
        _In_ ID3D11Device* pDevice,
        _In_ ID3D11DeviceContext* pImmediateContext,
        _In_ const aiScene* pScene,
        _In_ const std::filesystem::path& filePath
    )
    {
        HRESULT hr = S_OK;

        // Extract the directory part from the file name
        std::filesystem::path parentDirectory = filePath.parent_path();

        // Initialize the materials
        for (UINT i = 0u; i < pScene->mNumMaterials; ++i)
        {
            const aiMaterial* pMaterial = pScene->mMaterials[i];

            loadTextures(pDevice, pImmediateContext, parentDirectory, pMaterial, i);
        }

        return hr;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::initSingleMesh

      Summary:  Initialize single mesh from a given assimp mesh

      Args:     const aiMesh* pMesh
                  Point to an assimp mesh object
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Model::initSingleMesh definition (remove the comment)
    --------------------------------------------------------------------*/
    void Model::initSingleMesh(_In_ const aiMesh* pMesh)
    {
        const aiVector3D zero3d(0.0f, 0.0f, 0.0f);

        for (UINT i = 0u; i < pMesh->mNumVertices; ++i)
        {
            const aiVector3D& position = pMesh->mVertices[i];
            const aiVector3D& normal = pMesh->mNormals[i];
            const aiVector3D& texCoord = pMesh->HasTextureCoords(0u) ? pMesh->mTextureCoords[0][i] : zero3d;

            m_aVertices.push_back(
                SimpleVertex
                {
                    .Position = XMFLOAT3(position.x,position.y,position.z),
                    .TexCoord = XMFLOAT2(texCoord.x,texCoord.y),
                    .Normal = XMFLOAT3(normal.x, normal.y, normal.z)                        
                }
            );
        }

        for (UINT i = 0u; i < pMesh->mNumFaces; ++i)
        {
            const aiFace& face = pMesh->mFaces[i];
            assert(face.mNumIndices == 3u);

            m_aIndices.push_back(static_cast<WORD>(face.mIndices[0]));
            m_aIndices.push_back(static_cast<WORD>(face.mIndices[1]));
            m_aIndices.push_back(static_cast<WORD>(face.mIndices[2]));
        }
    }

    //???//
    /*void Model::loadColors(_In_ const aiMaterial* pMaterial, _In_ UINT uIndex)
    {

    }*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::loadDiffuseTexture

      Summary:  Load a diffuse texture from given path

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers
                const std::filesystem::path& parentDirectory
                  Parent path to the model
                const aiMaterial* pMaterial
                  Pointer to an assimp material object
                UINT uIndex
                  Index to a material
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Model::loadDiffuseTexture(
        _In_ ID3D11Device* pDevice, 
        _In_ ID3D11DeviceContext* pImmediateContext, 
        _In_ const std::filesystem::path& parentDirectory, 
        _In_ const aiMaterial* pMaterial, 
        _In_ UINT uIndex
        )
    {
        HRESULT hr = S_OK;
        m_aMaterials[uIndex].pDiffuse = nullptr;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString aiPath;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0u, &aiPath, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
            {
                std::string szPath(aiPath.data);

                if (szPath.substr(0ull, 2ull) == ".\\")
                {
                    szPath = szPath.substr(2ull, szPath.size() - 2ull);
                }

                std::filesystem::path fullPath = parentDirectory / szPath;

                m_aMaterials[uIndex].pDiffuse = std::make_shared<Texture>(fullPath);

                hr = m_aMaterials[uIndex].pDiffuse->Initialize(pDevice, pImmediateContext);
                if (FAILED(hr))
                {
                    OutputDebugString(L"Error loading diffuse texture \"");
                    OutputDebugString(fullPath.c_str());
                    OutputDebugString(L"\"\n");

                    return hr;
                }

                OutputDebugString(L"Loaded diffuse texture \"");
                OutputDebugString(fullPath.c_str());
                OutputDebugString(L"\"\n");
            }
        }

        return hr;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::loadSpecularTexture

      Summary:  Load a specular texture from given path

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers
                const std::filesystem::path& parentDirectory
                  Parent path to the model
                const aiMaterial* pMaterial
                  Pointer to an assimp material object
                UINT uIndex
                  Index to a material
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Model::loadSpecularTexture(
        _In_ ID3D11Device* pDevice, 
        _In_ ID3D11DeviceContext* pImmediateContext, 
        _In_ const std::filesystem::path& parentDirectory, 
        _In_ const aiMaterial* pMaterial, 
        _In_ UINT uIndex
        )
    {
        HRESULT hr = S_OK;
        m_aMaterials[uIndex].pSpecular = nullptr;

        if (pMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            aiString aiPath;

            if (pMaterial->GetTexture(aiTextureType_SHININESS, 0u, &aiPath, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
            {
                std::string szPath(aiPath.data);

                if (szPath.substr(0ull, 2ull) == ".\\")
                {
                    szPath = szPath.substr(2ull, szPath.size() - 2ull);
                }

                std::filesystem::path fullPath = parentDirectory / szPath;

                m_aMaterials[uIndex].pSpecular = std::make_shared<Texture>(fullPath);

                hr = m_aMaterials[uIndex].pSpecular->Initialize(pDevice, pImmediateContext);
                if (FAILED(hr))
                {
                    OutputDebugString(L"Error loading specular texture \"");
                    OutputDebugString(fullPath.c_str());
                    OutputDebugString(L"\"\n");

                    return hr;
                }

                OutputDebugString(L"Loaded specular texture \"");
                OutputDebugString(fullPath.c_str());
                OutputDebugString(L"\"\n");
            }
        }

        return hr;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::loadTextures

      Summary:  Load a specular texture from given path

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers
                const std::filesystem::path& parentDirectory
                  Parent path to the model
                const aiMaterial* pMaterial
                  Pointer to an assimp material object
                UINT uIndex
                  Index to a material
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Model::loadTextures(
        _In_ ID3D11Device* pDevice, 
        _In_ ID3D11DeviceContext* pImmediateContext, 
        _In_ const std::filesystem::path& parentDirectory, 
        _In_ const aiMaterial* pMaterial, 
        _In_ UINT uIndex
        )
    {
        HRESULT hr = loadDiffuseTexture(pDevice, pImmediateContext, parentDirectory, pMaterial, uIndex);
        if (FAILED(hr))
        {
            return hr;
        }

        hr = loadSpecularTexture(pDevice, pImmediateContext, parentDirectory, pMaterial, uIndex);
        if (FAILED(hr))
        {
            return hr;
        }

        return hr;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Model::reserveSpace

      Summary:  Reserve space for vertices and indices vectors

      Args:     UINT uNumVertices
                  Number of vertices
                UINT uNumIndices
                  Number of indices
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Model::reserveSpace definition (remove the comment)
    --------------------------------------------------------------------*/
    void Model::reserveSpace(_In_ UINT uNumVertices, _In_ UINT uNumIndices)
    {
        m_aVertices.reserve(uNumVertices);
        m_aIndices.reserve(uNumIndices);
    }
}