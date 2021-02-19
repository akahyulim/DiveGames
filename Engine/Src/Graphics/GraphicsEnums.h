#pragma once
#include "DivePch.h"


// 모든 enum이 define을 가질 필요는 없다.
// base class가 존재한다면 undefined
// 없다면 생성시 선택토록 하면 된다.
namespace Dive
{
	enum class eScreenMode
	{
		FullScreen,
		Borderless,
		Windowed,
	};

	enum class eRenderShaderType
	{
		Vertex_Color,

		Light_Directional,
		Light_Point,
		Light_Sopt,
	};

	enum class eRenderTextureType
	{
		EditorView,
		DepthStencil,
	};

	enum class eRasterizerState
	{
		CullBackSolid,
		CullFrontSolid,
		CullNoneSolid,
		CullBackWireFrame,
		CullFrontWireFrame,
		CullNoneWireFrame,
	};

	enum class eBlendState
	{
		Enabled,
		Disabled,
		ColorAdded,
	};

	enum class eSamplerType
	{
		Linear,
		CompareDepth,
		PointClamp,
		BilinearClamp,
		BilinearWrap,
		TrilinearClamp,
		AnisotropicWrap,
	};

	enum class eLightType
	{
		Directional,
		Point,
		Spot,
	};
}