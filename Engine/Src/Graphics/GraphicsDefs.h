#pragma once

namespace Dive
{
	enum class eGeometryType
	{
		Static,
		Skinned,
		Instanced,
		Billboard,
	};

	enum class eTextureUsage
	{
		Static,
		Dynamic,
		RenderTarget,
		DepthStencil
	};

	enum class eShaderType
	{
		Vertex,
		Pixel,
		None
	};

	// 좀 애매해서 아직 사용하지 않았다.
	enum eClearTarget : uint32_t
	{
		Color = 0x1,
		Depth = 0x2,
		Stencil = 0x4
	};

	// 타입을 변수로 삼아야 했다.
	enum eVertexElementType
	{
		TYPE_INT = 0,
		TYPE_FLOAT,
		TYPE_VECTOR2,
		TYPE_VECTOR3,
		TYPE_VECTOR4,
		TYPE_UBYTE4,
		TYPE_UBYTE4_NORM,
		MAX_VERTEX_ELEMENT_TYPES
	};

	// 이건 이미 존재할 수 있다.
	enum eVertexElementSemantic
	{
		SEM_POSITION = 0,
		SEM_NORMAL,
		SEM_BINORMAL,
		SEM_TANGENT,
		SEM_TEXCOORD,
		SEM_COLOR,
		SEM_BLENDWEIGHTS,
		SEM_BLENDINDICES,
		SEM_OBJECTINDEX,
		MAX_VERTEX_ELEMENT_SEMANTICS
	};

	struct VertexElement
	{
		VertexElement()
			: m_Type(TYPE_VECTOR3),
			m_Semantic(SEM_POSITION),
			m_Index(0),
			m_PerInstnace(false),
			m_Offset(0)
		{}

		VertexElement(eVertexElementType type, eVertexElementSemantic semantic, unsigned char index = 0, bool perInstance = false)
			: m_Type(type),
			m_Semantic(semantic),
			m_Index(index),
			m_PerInstnace(perInstance),
			m_Offset(0)
		{}

		eVertexElementType m_Type;
		eVertexElementSemantic m_Semantic;
		unsigned char m_Index;
		bool m_PerInstnace;
		unsigned int m_Offset;
	};

	// 정점 요소 데이터 크기 배열.
	const unsigned int ELEMENT_TYPESIZES[] =
	{
		sizeof(int),
		sizeof(float),
		2 * sizeof(float),
		3 * sizeof(float),
		4 * sizeof(float),
		sizeof(unsigned int),
		sizeof(unsigned int)
	};

	static const char* ELEMENT_SEMANTICNAMES[] =
	{
		"POSITION",
		"NORMAL",
		"BINORMAL",
		"TANGENT",
		"TEXCOORD",
		"COLOR",
		"BLENDWEIGHT",
		"BLENDINDICES",
		"OBJECTINDEX"
	};
	
	const DXGI_FORMAT ELEMENT_FORMATS[] =
	{
		DXGI_FORMAT_R32_SINT,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R8G8B8A8_UINT,
		DXGI_FORMAT_R8G8B8A8_UNORM
	};

	inline constexpr int32_t MAX_RENDERTARGETS = 4;
	inline constexpr int32_t MAX_VERTEX_STREAMS = 4;
}