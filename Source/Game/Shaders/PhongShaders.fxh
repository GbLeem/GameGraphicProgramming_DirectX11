//--------------------------------------------------------------------------------------
// File: PhongShaders.fx
//
// Copyright (c) Kyung Hee University.
//--------------------------------------------------------------------------------------

#define NUM_LIGHTS (2)

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

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
}
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnResize

  Summary:  Constant buffer used for projection transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;   
}

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangesEveryFrame

  Summary:  Constant buffer used for world transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 OutputColor;
}

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbLights

  Summary:  Constant buffer used for shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbLights : register(b3)
{
    float4 LightPositions[NUM_LIGHTS];
    float4 LightColors[NUM_LIGHTS];
}
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_PHONG_INPUT

  Summary:  Used as the input to the vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
struct VS_PHONG_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_PHONG_INPUT

  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
struct PS_PHONG_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_LIGHT_CUBE_INPUT

  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
struct PS_LIGHT_CUBE_INPUT
{
    float4 Position : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_PHONG_INPUT VSPhong(VS_PHONG_INPUT input)
{
    PS_PHONG_INPUT output = (PS_PHONG_INPUT)0;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.TexCoord = input.TexCoord;
    output.Normal = normalize( mul (float4 (input.Normal, 0) ,World).xyz );
    output.WorldPosition = mul( input.Position, World);

    return output;
}

PS_LIGHT_CUBE_INPUT VSLightCube(VS_PHONG_INPUT input)
{
    PS_LIGHT_CUBE_INPUT output = (PS_LIGHT_CUBE_INPUT)0;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSPhong(PS_PHONG_INPUT input) : SV_TARGET
{
   //ambient
   float3 ambient;

   for(uint i = 0; i < NUM_LIGHTS; ++i)
   {
       ambient += float3(0.2f, 0.2f, 0.2f) * LightColors[i].xyz * txDiffuse.Sample(samLinear, input.TexCoord);
   }

   //diffuse shading
   float3 diffuse; //if dot product result is negative number, clamp to 0 
   for(uint i = 0; i < NUM_LIGHTS; ++i)
   {
        float3 lightDirection = normalize(LightPositions[i].xyz - input.WorldPosition);
        diffuse += saturate(dot(input.Normal, lightDirection)) * LightColors[i].xyz * txDiffuse.Sample(samLinear, input.TexCoord);
   }

   //specular shading
   float3 viewDirection = normalize(CameraPosition.xyz - input.WorldPosition);
   float3 specular;

   for(uint i = 0; i < NUM_LIGHTS; ++i)
   {
        float3 lightDirection = normalize(LightPositions[i].xyz - input.WorldPosition);
        float3 reflectDirection = reflect(-lightDirection, input.Normal);
        specular += pow(saturate(dot(viewDirection, reflectDirection)), 20.0f) * LightColors[i].xyz * txDiffuse.Sample(samLinear, input.TexCoord);
   }

   return float4(ambient + diffuse + specular ,1.0f);
}

float4 PSLightCube(PS_LIGHT_CUBE_INPUT input) : SV_Target
{
    return OutputColor;
}

//--------------------------------------------------------------------------------------
// BlinnPhong shading 
//--------------------------------------------------------------------------------------
struct VS_BlinnPhong_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct PS_BlinnPhong_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
};

PS_BlinnPhong_INPUT VSBlinnPhong (VS_BlinnPhong_INPUT input)
{
    PS_BlinnPhong_INPUT output = (PS_BlinnPhong_INPUT)0;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.TexCoord = input.TexCoord;
    output.Normal = normalize( mul (float4 (input.Normal, 1.0f) ,World).xyz );
    output.WorldPosition = mul( input.Position, World);

    return output;
}

float4 PSBlinnPhong(PS_BlinnPhong_INPUT input) : SV_TARGET
{
   //ambient
	float3 ambient = float3(0.2f, 0.2f, 0.2f);

   //diffuse
   float3 diffuse;
   for (uint i = 0; i < NUM_LIGHTS; ++i)
   {
        float3 lightDirection = normalize(LightPositions[i].xyz - input.WorldPosition);
		diffuse += saturate(dot(input.Normal, lightDirection)) * LightColors[i].xyz;
	}

   //specular
   float3 specular;
   float3 h;
   float3 viewDirection = normalize(CameraPosition.xyz - input.WorldPosition);

   for (uint i = 0; i < NUM_LIGHTS; ++i)
   {
        float3 lightDirection = normalize(LightPositions[i].xyz - input.WorldPosition);

        h = normalize(lightDirection + viewDirection);
		specular += min(pow(saturate(dot(h, input.Normal)), 80.0f), 1.0f) * LightColors[i].xyz;
	}
   
	return float4(ambient + diffuse + specular, 1.0f) * txDiffuse.Sample(samLinear, input.TexCoord);
}


