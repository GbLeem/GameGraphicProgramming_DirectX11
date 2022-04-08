#pragma once
#include "Cube/BaseCube.h"

class SmallCube : public BaseCube
{
public:
	SmallCube() = default;
	~SmallCube() = default;
	void Update(_In_ FLOAT deltaTime) override;
};