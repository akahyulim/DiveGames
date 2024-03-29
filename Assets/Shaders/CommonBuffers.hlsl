// 공용이라면 Renderer3D 아니면 적어도 ForwardRenderer에서 만들고 관리해야 한다.
struct FrameBufferData
{
    matrix world;
    matrix view;
    matrix projection;
};

// 역시 공용이라 볼 수 없다.
// Material이 여러가지로 분화될 수 있기 때문이다.
struct MaterialBufferData
{
    float4 color;
    
    float2 tiling;
    float2 offset;
    
    uint properties;
    float3 padding;
};

struct CameraData
{
    float3 position;
    float p0;
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
};

// VS ConstantBuffers
cbuffer cbFrame : register(b0)
{ 
    FrameBufferData cbFrame;
}

// PS Constantuffers
cbuffer cbMaterial : register(b0)
{
    MaterialBufferData cbMaterial;
}

cbuffer cbCamera : register(b1)
{
    CameraData cbCamera;
}

cbuffer cbLight : register(b2)
{
    LightBufferData cbLight;
}

// cbLight options
bool IsDirectionalLight() 
{ 
    return cbLight.options & uint(1U << 0); 
}

bool IsPointLight()
{
    return cbLight.options & uint(1U << 1);
}

bool IsSpotLight()
{
    return cbLight.options & uint(1U << 2);
}

// cbMaterial options
bool HasDiffuseTexture()
{
    return cbMaterial.properties & uint(1U << 0);
}