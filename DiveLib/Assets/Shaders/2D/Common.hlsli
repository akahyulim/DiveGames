
// Constatnt Buffer
cbuffer FrameData : register(b0)
{
    float4x4 g_viewProjection;
};

// Samplers
SamplerState g_wrapPointSampler : register(s0);
SamplerState g_wrapLinearSampler : register(s1);
SamplerState g_clampPointSampler : register(s2);
SamplerState g_clampLinearSampler : register(s3);

// SRV
Texture2D g_spriteTex[32] : register(t0);
Texture2D<float4> g_screenRenderTarget : register(t32);