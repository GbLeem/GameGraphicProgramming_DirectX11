//--------------------------------------------------------------------------------------
// File: CubeMap.fxh
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Declare a diffuse texture and a sampler state (remove the comment)
--------------------------------------------------------------------*/
#define NUM_LIGHTS (1)

TextureCube txDiffuse : register(t0);
SamplerState clampSampler : register(s0);
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnCameraMovement

  Summary:  Constant buffer used for view transformation and shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangeOnCameraMovement definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangeOnCameraMovement : register(b0)
{
	matrix View;
	float4 CameraPosition;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnResize

  Summary:  Constant buffer used for projection transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangeOnResize definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangeOnResize : register(b1)
{
	matrix Projection;
};
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangesEveryFrame

  Summary:  Constant buffer used for world transformation, and the 
            color of the voxel
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangesEveryFrame definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangesEveryFrame : register(b2)
{
	matrix World;
	float4 OutputColor;
};

struct PointLight
{
	float4 Position;
	float4 Color;
	float4 AttenuationDistance;
};

cbuffer cbLights : register(b3)
{
	PointLight PointLights[NUM_LIGHTS];
};
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_INPUT

  Summary:  Used as the input to the vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: VS_INPUT definition (remove the comment)
--------------------------------------------------------------------*/
struct VS_INPUT
{
	float4 Position : POSITION;
};

struct VS_INPUT_ENV
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_INPUT

  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: PS_INPUT definition (remove the comment)
--------------------------------------------------------------------*/
struct PS_INPUT
{
	float4 Position : SV_Position;
	float3 TexCoord : TEXCOORD0;
};

struct PS_INPUT_ENV
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPosition : WORLDPOS;

};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Vertex Shader function VSCubeMap definition (remove the comment)
--------------------------------------------------------------------*/
PS_INPUT VSCubeMap(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT) 0;
	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);

	output.TexCoord = input.Position.xyz;

	return output;
}

PS_INPUT_ENV VSEnvironmentMap(VS_INPUT_ENV input)
{
	PS_INPUT_ENV output = (PS_INPUT_ENV) 0;
	
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
    
	output.WorldPosition = mul(input.Pos, World);

	output.Tex = input.Tex;
	
	output.Normal = normalize(mul(float4(input.Normal, 0.f), World).xyz);
    
	return output;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Pixel Shader function PSCubeMap definition (remove the comment)
--------------------------------------------------------------------*/
float4 PSCubeMap(PS_INPUT input) : SV_Target
{
	return txDiffuse.Sample(clampSampler, input.TexCoord);
}

float4 PSEnvironmentMap(PS_INPUT_ENV input):SV_Target
{
	float3 reflectDirection = float3(0.f, 0.f, 0.f);
	float3 incident = normalize(input.WorldPosition - CameraPosition.xyz);
	
	reflectDirection = reflect(incident, input.Normal);
	
	return txDiffuse.Sample(clampSampler, reflectDirection.xyz);
}