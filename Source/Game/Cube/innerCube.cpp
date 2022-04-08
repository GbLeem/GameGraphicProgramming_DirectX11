#include "innerCube.h"

void innerCube::Update(_In_ FLOAT deltaTime)
{
	XMMATRIX mSpin = XMMatrixRotationZ(deltaTime);
	XMMATRIX mOrbit = XMMatrixRotationY(-deltaTime * 3.0f);
	XMMATRIX mTranslate = XMMatrixTranslation(-3.0f, 0.0f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	m_world = mScale * mSpin * mTranslate * mOrbit;
}
