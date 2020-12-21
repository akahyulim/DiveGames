
cbuffer GlobalBuffer : register(b0)
{
	matrix g_wvp;
	matrix g_view;
	matrix g_viewInverse;
	matrix g_viewBase;
	matrix g_proj;
	matrix g_projOrtho;
	matrix g_viewProj;
	matrix g_viewProjInverse;
	matrix g_viewProjOrtho;

	float4 g_perspectiveValues;

	float g_nearPlane;
	float g_farPlane;
	float2 g_resolution;

	float3 g_cameraPos;
	float g_deltaTime;
}