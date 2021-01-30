
#include "Common_Buffer.hlsl"
#include "Common_Params.hlsl"


float3 CalcuWorldPosition(float depth, float2 texCoord)
{
	float x			= texCoord.x * 2.0f - 1.0f;
	float y			= (1.0f - texCoord.y) * 2.0f - 1.0f;
	float4 posClip	= float4(x, y, depth, 1.0f);
	float4 posWorld = mul(posClip, g_viewProjInverse);

	return posWorld.xyz / posWorld.w;
}