#pragma once

#include "Common.h"

#include"Cube/BaseCube.h"

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Class:    FireCube

  Summary:  A renderable 3d cube object

  Methods:  Update
              Overriden function that updates the cube every frame
            Cube
              Constructor.
            ~Cube
              Destructor.
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
class FireCube : public BaseCube
{
public:
    FireCube(const std::filesystem::path& textureFilePath);
    FireCube(const FireCube& other) = delete;
    FireCube(FireCube&& other) = delete;
    FireCube& operator=(const FireCube& other) = delete;
    FireCube& operator=(FireCube&& other) = delete;
    ~FireCube() = default;

    virtual void Update(_In_ FLOAT deltaTime) override;
};