// 공용이라면 Renderer3D 아니면 적어도 ForwardRenderer에서 만들고 관리해야 한다.
struct ModelBufferData
{
    matrix world;
};

struct CameraVS
{
    matrix view;
    matrix projection;
};

// 어쩔 수 없이 셰이더별로 분리해야한다.
struct LightVS
{
    matrix shadow;
};

// 역시 공용이라 볼 수 없다.
// Material이 여러가지로 분화될 수 있기 때문이다.
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

// naming을 cbLightVertex 이런식으로 하자.
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