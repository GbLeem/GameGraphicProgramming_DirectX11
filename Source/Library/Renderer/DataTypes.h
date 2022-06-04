#pragma once

#include "Common.h"

namespace library
{
#define NUM_LIGHTS (1)
#define MAX_NUM_BONES (256)
#define MAX_NUM_BONES_PER_VERTEX (16)

	/*S+S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S
	 Struct:   NormalData
	 Summary:  NormalData structure containing tangent space vetors
			   of the vertex
   S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S-S*/
	struct NormalData
	{
		XMFLOAT3 Tangent;
		XMFLOAT3 Bitangent;
	};

	struct SimpleVertex
	{
		XMFLOAT3 Position;
		XMFLOAT2 TexCoord;
		XMFLOAT3 Normal;
	};

	struct InstanceData
	{
		XMMATRIX Transformation;
	};

	struct AnimationData
	{
		XMUINT4 aBoneIndices;
		XMFLOAT4 aBoneWeights;
	};

	struct CBChangeOnCameraMovement
	{
		XMMATRIX View;
		XMFLOAT4 CameraPosition;
	};

	struct CBChangeOnResize
	{
		XMMATRIX Projection;
	};

	struct CBChangesEveryFrame
	{
		XMMATRIX World;
		XMFLOAT4 OutputColor;
		BOOL HasNormalMap;
	};

	struct CBSkinning
	{
		XMMATRIX BoneTransforms[MAX_NUM_BONES];
	};

	struct Lights
	{
		XMFLOAT4 Position;
		XMFLOAT4 Color;
		XMFLOAT4 AttenuationDistance;
	};

	struct CBLights
	{
		//XMFLOAT4 LightPositions[NUM_LIGHTS];
		//XMFLOAT4 LightColors[NUM_LIGHTS];
		//XMMATRIX LightViews[NUM_LIGHTS];
		//XMMATRIX LightProjections[NUM_LIGHTS];
		Lights PointLights[NUM_LIGHTS];
	};

	/*struct CBShadowMatrix
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
		BOOL IsVoxel;
	};*/
}