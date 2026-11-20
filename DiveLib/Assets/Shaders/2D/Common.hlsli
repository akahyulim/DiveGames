
// Constatnt Buffer
cbuffer FrameData : register(b0)
{
    float4x4 viewProjection;
};

// Samplers
SamplerState WrapPointSampler : register(s0);
SamplerState WrapLinearSampler : register(s1);
SamplerState ClampPointSampler : register(s2);
SamplerState ClampLinearSampler : register(s3);

// SRV
Texture2D SpriteTex : register(t0);
Texture2D<float4> ScreenRenderTarget : register(t1);