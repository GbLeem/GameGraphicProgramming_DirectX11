#pragma once
#include "Cube/BaseCube.h"

class innerCube : public BaseCube
{	
public:
	innerCube() = default;
	~innerCube() = default;
	void Update(_In_ FLOAT deltaTime) override;
};

