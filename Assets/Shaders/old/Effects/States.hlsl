//= Samplers ==========================
SamplerState samplerLinear
{
	Filter		= MIN_MAG_MIP_LINEAR;
	AddressU	= Wrap;
	AddressV	= Wrap;
};

//= Depth =============================
DepthStencilState DepthEnabled
{
	DepthEnable		= TRUE;
	DepthWriteMask	= ALL;
	DepthFunc		= LESS_EQUAL;
};

//= Blend =============================
BlendState BlendDisabled
{
	AlphaToCoverageEnable	= FALSE;
	BlendEnable[0]			= FALSE;
};

//= Rasterizer ========================
RasterizerState CullBackSolid
{
	CullMode = BACK;
	FillMode = SOLID;
};

RasterizerState CullBackWireFrame
{
	CullMode = BACK;
	FillMode = WIREFRAME;
};
