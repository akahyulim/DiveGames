#pragma once
#include "DivePch.h"


// ��� enum�� define�� ���� �ʿ�� ����.
// base class�� �����Ѵٸ� undefined
// ���ٸ� ������ ������� �ϸ� �ȴ�.
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