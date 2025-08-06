
// Vertex Shader ====================================================
struct MatrixData
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projMatrix;    
};

cbuffer MatrixBuffer : register(b0)
{
    MatrixData cb_Matrix;
}

// Pixel Shader =====================================================
struct MaterialData
{
    float4 diffuseColor;
    float2 tiling;
    float2 offset;
    uint properties;
    uint3 padding;
};

cbuffer MaterialBuffer : register(b0)
{
    MaterialData cb_Material;
}

bool HasDiffuseMap()
{
    return cb_Material.properties & uint(1U << 0);
}

bool HasNormalMap()
{
    return cb_Material.properties & uint(1U << 1);
}