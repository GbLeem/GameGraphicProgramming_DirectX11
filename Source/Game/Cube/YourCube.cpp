#include "YourCube.h"

void YourCube::Update(_In_ FLOAT deltaTime)
{
    m_world = XMMatrixRotationY(deltaTime);
}

