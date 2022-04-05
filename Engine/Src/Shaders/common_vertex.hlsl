
struct Vertex_PosColTex
{
	float4 position : POSITION0;
	float4 color	: COLOR0;
	float2 texCoord : TEXCOORD0;
};

struct Vertex_PosTexNorTan
{
	float4 position : POSITION0;
	float2 texCoord : TEXCOORD0;
	float3 normal	: NORMAL0;
	float3 tangent	: TANGENT0;
};