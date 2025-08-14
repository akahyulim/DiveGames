
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

// Pixel Shader =====================================================
struct CameraData
{
    float3 position;
    float padding;
    float4 perspectiveValue;
    matrix viewInverse;
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
    float3 color;
    float outerConeAngle;
    
    float3 position;
    float rangeRcp;
    
    float3 direction;
    float innerConeAngle;
    
    uint options;
    int shadowEnabled;
    float2 padding;
    
    matrix shadow;
};

cbuffer LightBuffer : register(b2)
{
    LightData cbLight;
}

// cbLightPixel options
bool IsDirectionalLight()
{
    return (cbLight.options & 1) != 0;
}
bool IsPointLight()
{
    return (cbLight.options & 2) != 0;
}
bool IsSpotLight()
{
    return (cbLight.options & 4) != 0;
}
