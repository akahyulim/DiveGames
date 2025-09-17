#define MAX_LIGHTS 16


// Vertex Shader ====================================================
struct MatrixData
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projMatrix;    
};

cbuffer MatrixBuffer : register(b0)
{
    MatrixData cbMatrix;
}

struct CameraDataVS
{
    matrix viewMatrix;
    matrix projMatrix;
};

cbuffer CameraVSBuffer : register(b1)
{
    CameraDataVS cbCameraVS;
}

// Pixel Shader =====================================================
struct CameraData
{
    float3 position;
    float padding;
    float4 backgroundColor;
};

cbuffer CameraBuffer : register(b0)
{
    CameraData cbCamera;
}

struct MaterialData
{
    float4 diffuseColor;
    float2 tiling;
    float2 offset;
    uint properties;
    uint3 padding;
};

cbuffer MaterialBuffer : register(b1)
{
    MaterialData cbMaterial;
}

bool HasDiffuseMap()
{
    return cbMaterial.properties & uint(1U << 0);
}

bool HasNormalMap()
{
    return cbMaterial.properties & uint(1U << 1);
}

struct LightData
{
    uint type;
    float3 color;

    float intensity;
    float3 position;

    float rangeRcp;
    float3 direction;

    float innerAngle;
    float outerAngle;
    float2 padding;
};

struct ForwardLight
{
    float4 ambientColor;
    int lightCount;
    float3 padding;
    LightData lights[MAX_LIGHTS];
};

cbuffer ForwardLightBuffer : register(b2)
{
    ForwardLight cbForwardLight;
}