//--------------------------------------------------------------------------------------
// File: PhongShaders.fxh
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

#define NUM_LIGHTS (2)

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Declare texture array and sampler state array for diffuse texture and normal texture (remove the comment)
--------------------------------------------------------------------*/
Texture2D aTextures[2] : register(t0);
SamplerState aSamplers[2] : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnCameraMovement
  Summary:  Constant buffer used for view transformation and shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangeOnCameraMovement : register(b0)
{
	matrix View;
	float4 CameraPosition;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnResize
  Summary:  Constant buffer used for projection transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
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
	bool HasNormalMap;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbLights
  Summary:  Constant buffer used for shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbLights : register(b3)
{
	float4 LightPositions[NUM_LIGHTS];
	float4 LightColors[NUM_LIGHTS];
};
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_PHONG_INPUT

  Summary:  Used as the input to the vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: VS_PHONG_INPUT definition (remove the comment)
--------------------------------------------------------------------*/
struct VS_PHONG_INPUT
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
	row_major matrix mTransform : INSTANCE_TRANSFORM;
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
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Vertex Shader function VSPhong definition (remove the comment)
--------------------------------------------------------------------*/
PS_PHONG_INPUT VSPhong(VS_PHONG_INPUT input)
{
	PS_PHONG_INPUT output = (PS_PHONG_INPUT) 0;
	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	output.Normal = normalize(mul(float4(input.Normal, 0), World).xyz);
	
	output.TexCoord = input.TexCoord;
	output.WorldPosition = mul(input.Position, World);
	
	if (HasNormalMap)
	{
		output.Tangent = normalize(mul(float4(input.Tangent, 0.0f), World).xyz);
		output.Bitangent = normalize(mul(float4(input.Bitangent, 0.0f), World).xyz);
	}
	
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
	float3 normal = normalize(input.Normal);
	
	if (HasNormalMap)
	{
		//sample the pixel in the normal map
		float4 bumpMap = aTextures[1].Sample(aSamplers[1], input.TexCoord);
		
		//expand the range of the normal value to -1~1
		bumpMap = (bumpMap * 2.0f) - 1.0f;

		//calculate the normal from the data in normal map
		float bumpNormal = (bumpMap.x * input.Tangent) + (bumpMap.y * input.Bitangent) + (bumpMap.z * input.Normal);

		//normalize the resulting bump normal and replace existing normal
		normal = normalize(bumpNormal);
	}
	
   //ambient
	float3 ambient = float3(0.f, 0.f, 0.f);

	for (uint i = 0; i < NUM_LIGHTS; ++i)
	{
		ambient += float3(0.2f, 0.2f, 0.2f) * LightColors[i].xyz;
	}

   //diffuse shading
	float3 diffuse = float3(0.f, 0.f, 0.f);
	for (uint j = 0; j < NUM_LIGHTS; ++j)
	{
		float3 lightDirection = normalize(LightPositions[j].xyz - input.WorldPosition);
		diffuse += saturate(dot(/*input.Normal*/normal, lightDirection)) * LightColors[j].xyz;
	}

   //specular shading
	float3 viewDirection = normalize(CameraPosition.xyz - input.WorldPosition);
	float3 specular = float3(0.f, 0.f, 0.f);

	for (uint k = 0; k < NUM_LIGHTS; ++k)
	{
		float3 lightDirection = normalize(LightPositions[k].xyz - input.WorldPosition);
		float3 reflectDirection = reflect(-lightDirection, /*input.Normal*/normal);
		specular += pow(saturate(dot(viewDirection, reflectDirection)), 20.0f) * LightColors[k].xyz;
	}

	return float4(ambient + diffuse + specular, 1.0f) * aTextures[0].Sample(aSamplers[0], input.TexCoord);
}