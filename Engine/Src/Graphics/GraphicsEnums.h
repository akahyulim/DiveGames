#pragma once
#include "DivePch.h"

// 구조체도 있는데 Enums라는 이름이다.
// 구조체를 따로 빼낼까...
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

	// 전부 지원하지 않는 값을 추가해야 한다.
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

	// Light로 옮기기
	enum class eLightType
	{
		Directional,
		Point,
		Spot,
	};
}