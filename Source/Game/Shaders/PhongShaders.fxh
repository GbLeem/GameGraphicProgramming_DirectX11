//--------------------------------------------------------------------------------------
// File: PhongShaders.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
#define NUM_LIGHTS (1)
#define NEAR_PLANE (0.01f)
#define FAR_PLANE (1000.0f)

Texture2D aTextures[2] : register(t0);
SamplerState aSamplers[2] : register(s0);

cbuffer cbChangeOnCameraMovement : register(b0)
{
	matrix View;
	float4 CameraPosition;
};
cbuffer cbChangeOnResize : register(b1)
{
	matrix Projection;
};
cbuffer cbChangesEveryFrame : register(b2)
{
	matrix World;
	float4 OutputColor;
	bool HasNormalMap;

};

//Light Struct for cbLights
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

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
};

struct PS_PHONG_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPosition : WORLDPOS;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
};

struct PS_LIGHT_CUBE_INPUT
{
	float4 Position : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_PHONG_INPUT VSPhong(VS_INPUT input)
{
	/*--------------------------------------------------------------------
	  TODO: Vertex shader code (remove the comment)
	--------------------------------------------------------------------*/
	PS_PHONG_INPUT output = (PS_PHONG_INPUT) 0;
	
	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	
	output.TexCoord = input.TexCoord;
	
	output.Normal = normalize(mul(float4(input.Normal, 0), World).xyz);
	
	output.WorldPosition = mul(input.Position, World);

	if (HasNormalMap)
	{
		output.Tangent = normalize(mul(float4(input.Tangent, 0.0f), World).xyz);
		output.Bitangent = normalize(mul(float4(input.Bitangent, 0.0f), World).xyz);
	}
	
	return output;
}

PS_LIGHT_CUBE_INPUT VSLightCube(VS_INPUT input)
{
	PS_LIGHT_CUBE_INPUT output = (PS_LIGHT_CUBE_INPUT) 0;
	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
    
	return output;
}

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return ((2.0 * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE))) / FAR_PLANE;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSPhong(PS_PHONG_INPUT input) : SV_Target
{
	/*--------------------------------------------------------------------
	  TODO: Pixel shader code (remove the comment)
	--------------------------------------------------------------------*/
	
	float3 normal = normalize(input.Normal);
	
	if (HasNormalMap)
	{
		//sample the pixel in the normal map
		float4 bumpMap = aTextures[1].Sample(aSamplers[1], input.TexCoord);
		
		//expand the range of the normal value to -1~1
		bumpMap = (bumpMap * 2.0f) - 1.0f;

		//calculate the normal from the data in normal map
		float3 bumpNormal = (bumpMap.x * input.Tangent) + (bumpMap.y * input.Bitangent) + (bumpMap.z * normal);

		//normalize the resulting bump normal and replace existing normal
		normal = normalize(bumpNormal);
	}
	
	float3 ambient = float3(0.f, 0.f, 0.f);
	float3 diffuse = float3(0.f, 0.f, 0.f);
	float3 specular = float3(0.f, 0.f, 0.f);
	
	float3 viewDir = normalize(CameraPosition.xyz - input.WorldPosition);
	
	for (uint i = 0; i < NUM_LIGHTS; ++i)
	{
		float3 lightDirection = normalize(PointLights[i].Position.xyz - input.WorldPosition);
		float3 reflectDirection = reflect(-lightDirection, normal);
	
		//get r^2
		float r = dot(input.WorldPosition - PointLights[i].Position.xyz, input.WorldPosition - PointLights[i].Position.xyz);
		
		//get r0^2
		float r0 = PointLights[i].AttenuationDistance.z;
		
		//get attenuation
		float attenuation = r0 / (r + 0.000001f);
		
		ambient += float3(0.1f, 0.1f, 0.1f) * PointLights[i].Color.xyz * attenuation;
		diffuse += saturate(dot(normal, lightDirection)) * PointLights[i].Color.xyz * attenuation;
		specular += pow(saturate(dot(viewDir, reflectDirection)), 20.f) * PointLights[i].Color.xyz * attenuation;
	}
	
	return float4(ambient + diffuse + specular, 1.0f) * aTextures[0].Sample(aSamplers[0], input.TexCoord);
}

float4 PSLightCube(PS_LIGHT_CUBE_INPUT input) : SV_Target
{
	return OutputColor;
}