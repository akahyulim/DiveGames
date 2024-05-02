// �����̶�� Renderer3D �ƴϸ� ��� ForwardRenderer���� ����� �����ؾ� �Ѵ�.
struct ModelBufferData
{
    matrix world;
};

struct CameraVS
{
    matrix view;
    matrix projection;
};

// ��¿ �� ���� ���̴����� �и��ؾ��Ѵ�.
struct LightVS
{
    matrix shadow;
};

// ���� �����̶� �� �� ����.
// Material�� ���������� ��ȭ�� �� �ֱ� �����̴�.
struct MaterialBufferData
{
    float4 color;
    float4 normal;
    
    float2 tiling;
    float2 offset;
    
    uint properties;
    float3 padding;
};

struct CameraData
{
    float3 position;
    float padding;
    
    float4 perspectiveValue;
    
    matrix viewInverse;
};

struct LightBufferData
{
    float3 color;
    float outerConeAngle;
    
    float3 position;
    float rangeRcp;
    
    float3 direction;
    float innerConeAngle;
    
    uint options;
    float3 padding;
    
    matrix shadow;
};

// VS ConstantBuffers
cbuffer cbModelVertex : register(b0)
{ 
    ModelBufferData cbModelVertex;
}

cbuffer cbCameraVertex : register(b1)
{
    CameraVS cbCameraVertex;
}

// naming�� cbLightVertex �̷������� ����.
cbuffer cbLightVertex : register(b2)
{
    LightVS cbLightVertex;
}

// PS Constantuffers
cbuffer cbMaterialPixel : register(b0)
{
    MaterialBufferData cbMaterialPixel;
}

cbuffer cbCameraPixel : register(b1)
{
    CameraData cbCameraPixel;
}

cbuffer cbLightPixel : register(b2)
{
    LightBufferData cbLightPixel;
}

// cbLightPixel options
bool IsDirectionalLight() 
{ 
    return cbLightPixel.options & uint(1U << 0); 
}

bool IsPointLight()
{
    return cbLightPixel.options & uint(1U << 1);
}

bool IsSpotLight()
{
    return cbLightPixel.options & uint(1U << 2);
}

// cbMaterialPixel options
bool HasDiffuseTexture()
{
    return cbMaterialPixel.properties & uint(1U << 0);
}

bool HasNormalTexture()
{
    return cbMaterialPixel.properties & uint(1U << 1);
}