//--------------------------------------------------------------------------------------
// File: ShadowShaders.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbShadowMatrix : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	bool isVoxel;
}

struct VS_SHADOW_INPUT
{
	float4 Position : POSITION;
    row_major matrix mTransform : INSTANCE_TRANSFORM;
};


struct PS_SHADOW_INPUT
{
    float4 Position : SV_POSITION;
	float4 DepthPosition : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_SHADOW_INPUT VSShadow(VS_SHADOW_INPUT input)
{
	/*--------------------------------------------------------------------
	  TODO: Vertex shader code (remove the comment)
	--------------------------------------------------------------------*/
	PS_SHADOW_INPUT output = (PS_SHADOW_INPUT) 0;
	float4 pos = input.Position;
	
	if (isVoxel)
	{
		pos = mul(input.Position, input.mTransform);
	}
	
	output.Position = mul(pos, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	
	output.DepthPosition = output.Position;
	
	return output;
};


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSShadow(PS_SHADOW_INPUT input) : SV_Target
{
	/*--------------------------------------------------------------------
	  TODO: Pixel shader code (remove the comment)
	--------------------------------------------------------------------*/
	float depthValue = input.DepthPosition.z / input.DepthPosition.w;
	return float4(depthValue, depthValue, depthValue, 1.0f);
};