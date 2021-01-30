#pragma once
#include "DivePch.h"

// ����ü�� �ִµ� Enums��� �̸��̴�.
// ����ü�� ���� ������...
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

	};

	enum class eRenderableObjectType
	{
		Camera,
		Light,
		Opaque,
		Transparent,
	};

	enum class eRenderTextureType
	{
		EditorView,
		DepthStencil,
	};

	// ���� �������� �ʴ� ���� �߰��ؾ� �Ѵ�.
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

	// Light�� �ű��
	enum class eLightType
	{
		Directional,
		Point,
		Spot,
	};
}