#pragma once

namespace Dive
{
	enum class eShaderType
	{
		Vertex,
		Pixel
	};

	// Ÿ���� ������ ��ƾ� �ߴ�.
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

	// �̰� �̹� ������ �� �ִ�.
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
		VertexElement(eVertexElementType type, eVertexElementSemantic semantic, unsigned char index = 0)
			: m_Type(type),
			m_Semantic(semantic),
			m_Index(index),
			m_Offset(0)
		{}

		eVertexElementType m_Type;
		eVertexElementSemantic m_Semantic;
		unsigned char m_Index;
		unsigned int m_Offset;
	};

	// ���� ��� ������ ũ�� �迭.
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
}