#define MAX_LIGHTS 16

// Common ===========================================================
struct Camera
{
    matrix viewMatrix;
    matrix projMatrix;
    matrix viewProjMatrix;
    float4 position;
    float4 backgroundColor;
};

cbuffer CameraBuffer : register(b0)
{
    Camera cbCamera;
}

// Vertex Shader ====================================================
struct ObjectVS
{
    matrix worldMatrix;
};

cbuffer ObjectBuffer : register(b1)
{
    ObjectVS cbObjectVS;
}


// Pixel Shader =====================================================
struct MaterialPS
{
    float4 diffuseColor;
    float2 tiling;
    float2 offset;
    uint flags;
    uint3 padding;
};

cbuffer MaterialBuffer : register(b1)
{
    MaterialPS cbMaterialPS;
}

bool HasDiffuseMap()  { return cbMaterialPS.flags & (1 << 0); }
bool HasNormalMap()   { return cbMaterialPS.flags & (1 << 1); }

struct Light
{
    uint type;
    float3 color;
    float intensity;
    float3 position;
    float rangeRcp;
    float3 direction;
    float innerAngle;
    float outerAngle;
};

struct ForwardLightPS
{
    float4 ambientColor;
    int lightCount;
    Light lights[MAX_LIGHTS];
};

cbuffer ForwardLightBuffer : register(b2)
{
    ForwardLightPS cbForwardLightPS;
}