/*
	제작: 서보윤( akahyulim@gmail.com )
*/

struct Vertex_Pos
{
	float4 position : POSITION0;
};

struct Vertex_PosCol
{
	float4 position : POSITION0;
	float4 color	: COLOR0;
};

struct Vertex_PosTex
{
	float4 position : POSITION0;
	float2 uv		: TEXCOORD0;
};

struct Vertex_PosTexNorTan
{
	float4 position : POSITION0;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL0;
	float3 tangent	: TANGENT0;
};

struct Pixel_Pos
{
	float4 position : SV_POSITION;
};

struct Pixel_PosCol
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
};

struct Pixel_PosTex
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
};

struct Pixel_PosTexNorTan
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: TEXCOORD1;
	float3 tangent	: TEXCOORD2;
};

//= test용으로 추가 ======================================
// Light.hlsl에 다른 타입이 존재하므로 일단 제외
//struct Pixel_PosTexNor
//{
//	float4 position : SV_POSITION;
//	float2 uv		: TEXCOORD0;
//	float3 normal	: TEXCOORD1;
//};

struct Pixel_PosTexNorWorldPos
{
	float4 position	: SV_POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: TEXCOORD1;
	float3 worldPos	: TEXCOORD2;
};
//========================================================
