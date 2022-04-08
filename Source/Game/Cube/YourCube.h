#pragma once

#include "Cube/BaseCube.h"

class YourCube : public BaseCube
{
public:
	YourCube() = default;
	~YourCube() = default;
	void Update(_In_ FLOAT deltaTime) override;
};


