/*
	¡¶¿€: º≠∫∏¿±( akahyulim@gmail.com )
*/

cbuffer GlobalBuffer : register(b0)
{
	matrix g_wvp;
	matrix g_view;
	matrix g_viewBase;
	matrix g_proj;
	matrix g_projOrtho;
	matrix g_viewProj;
	matrix g_viewProjInverse;
	matrix g_viewProjOrtho;

	matrix g_viewInverse;
	float4 g_perspectiveValues;

	float g_planeNear;
	float g_planeFar;
	float2 g_resolution;
	
	float3 g_cameraPos;
	float g_deltaTime;
};