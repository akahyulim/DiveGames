cbuffer FrameData : register(b0)
{
    float4x4 viewProjection;
};

cbuffer ObjectData : register(b1)
{
    float4x4 model;
};

struct VSInput
{
    float4 Position : POSITION;
    float4 Color : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
};

struct VSToPS
{
    float4 Position : SV_POSITION;
    float4 Color : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
};

VSToPS MainVS(VSInput input)
{
    VSToPS output;
    
    float4 position = input.Position;
    position.w = 1.0f;
    
    output.Position = mul(position, model);
    output.Position = mul(output.Position, viewProjection);
    output.Color = input.Color;
    output.Normal = mul(input.Normal, (float3x3)model);
    output.Normal = normalize(output.Normal);
    output.Tangent = mul(input.Tangent, (float3x3)model);
    output.Tangent = normalize(output.Tangent);
    output.BiNormal = mul(input.BiNormal, (float3x3)model);
    output.BiNormal = normalize(output.BiNormal);
    
    return output;
}

float4 MainPS(VSToPS input) : SV_TARGET0
{
    // 1. 빛의 방향 설정 (오른쪽 위 앞쪽에서 비추는 방향)
    float3 lightDir = normalize(float3(-0.5f, 1.0f, 0.5f));
    
    // 2. 노멀과 빛 방향의 내적을 이용해 디퓨즈(Diffuse) 광량 계산 (0 ~ 1)
    float3 normal = normalize(input.Normal);
    float diff = max(dot(normal, lightDir), 0.0f);
    
    // 3. 그림자 진 곳이 너무 까맣게 묻히지 않도록 환경광(Ambient) 추가 (예: 0.35)
    float ambient = 0.35f;
    float lighting = saturate(diff + ambient);
    
    // 4. 원래 색상에 조명 비율을 곱해서 최종 색상 결정
    float3 finalColor = input.Color.rgb * lighting;
    
    return float4(finalColor, input.Color.a);
    //return input.Color;
    //return float4(normal * 0.5f + 0.5f, 1.0f);
}
