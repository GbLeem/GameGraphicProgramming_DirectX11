//--------------------------------------------------------------------------------------
// File: SkinningShaders.fx
//
// Copyright (c) Microsoft Corporation.
//--------------------------------------------------------------------------------------
#define NUM_LIGHTS (2)

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
static const unsigned int MAX_NUM_BONES = 256u;
/*--------------------------------------------------------------------
  TODO: Declare a diffuse texture and a sampler state (remove the comment)
--------------------------------------------------------------------*/
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnCameraMovement

  Summary:  Constant buffer used for view transformation
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

  Summary:  Constant buffer used for world transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangesEveryFrame definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 OutputColor;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbLights

  Summary:  Constant buffer used for shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbLights definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbLights : register(b3)
{
    float4 LightPositions[NUM_LIGHTS];
    float4 LightColors[NUM_LIGHTS];
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbSkinning

  Summary:  Constant buffer used for skinning
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbLights definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbSkinning : register(b4)
{
    matrix BoneTransforms[MAX_NUM_BONES];
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
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	uint4 BoneIndices : BONEINDICES;
	float4 BoneWeights : BONEWEIGHTS;
};
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_PHONG_INPUT

  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: PS_PHONG_INPUT definition (remove the comment)
--------------------------------------------------------------------*/
struct PS_PHONG_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPosition : WORLDPOS;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Vertex Shader function VSPhong definition (remove the comment)
--------------------------------------------------------------------*/
PS_PHONG_INPUT VSPhong(VS_INPUT input)
{
	PS_PHONG_INPUT output = (PS_PHONG_INPUT) 0;
	matrix skinTransform = (matrix)0;
	
	skinTransform += BoneTransforms[input.BoneIndices.x] * input.BoneWeights.x;
	skinTransform += BoneTransforms[input.BoneIndices.y] * input.BoneWeights.y;
	skinTransform += BoneTransforms[input.BoneIndices.z] * input.BoneWeights.z;
	skinTransform += BoneTransforms[input.BoneIndices.w] * input.BoneWeights.w;
	
	output.Position = mul(input.Position, skinTransform);
	output.Position = mul(output.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	
	output.TexCoord = input.TexCoord;
	
	output.Normal = normalize(mul(float4(input.Normal, 0), skinTransform).xyz);
	output.Normal = normalize(mul(float4(input.Normal, 0), World).xyz);

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Pixel Shader function PSPhong definition (remove the comment)
--------------------------------------------------------------------*/
float4 PSPhong(PS_PHONG_INPUT input) : SV_TARGET
{
   //ambient
	float3 ambient = float3(0.f, 0.f, 0.f);

	for (uint i = 0; i < NUM_LIGHTS; ++i)
	{
		ambient += float3(0.2f, 0.2f, 0.2f) * LightColors[i].xyz;
	}

   //diffuse shading
	float3 diffuse = float3(0.f, 0.f, 0.f); 
	for (uint i = 0; i < NUM_LIGHTS; ++i)
	{
		float3 lightDirection = normalize(LightPositions[i].xyz - input.WorldPosition);
		diffuse += saturate(dot(input.Normal, lightDirection)) * LightColors[i].xyz;
	}

   //specular shading
	float3 viewDirection = normalize(CameraPosition.xyz - input.WorldPosition);
	float3 specular = float3(0.f, 0.f, 0.f);

	for (uint i = 0; i < NUM_LIGHTS; ++i)
	{
		float3 lightDirection = normalize(LightPositions[i].xyz - input.WorldPosition);
		float3 reflectDirection = reflect(-lightDirection, input.Normal);
		specular += pow(saturate(dot(viewDirection, reflectDirection)), 20.0f) * LightColors[i].xyz;
	}

	return float4(ambient + diffuse + specular, 1.0f) * txDiffuse.Sample(samLinear, input.TexCoord);
}