#pragma once
#include "Core/Object.h"
#include "VertexTypes.h"
#include "Core/Log.h"


namespace Dive
{
	class Context;

	class InputLayout : public Object
	{
		DIVE_OBJECT(InputLayout, Object);

	public:
		InputLayout(Context* context);
		~InputLayout();

		template<typename T>
		bool Create(ID3D10Blob* blob)
		{
			switch (VertexTypeToEnum<T>())
			{
			case eVertexType::Position:
				createPositionDesc();
				break;

			case eVertexType::PositionColor:
				createPositionDesc();
				createColorDesc();
				break;

			case eVertexType::PositionUv:
				createPositionDesc();
				createTextureDesc();
				break;

			case eVertexType::PositionUvNormalTangent:
				createPositionDesc();
				createTextureDesc();
				createNormalDesc();
				createTangentDesc();
				break;

			case eVertexType::PositionUvColor2D:
				create2dDesc();
				break;

			default:
				CORE_ERROR("");
				return false;
			}

			return createBuffer(blob);
		}

		ID3D11InputLayout* GetBuffer()	const { return m_buffer; }
		eVertexType GetVertexType()		const{ return m_vertexType; }

	private:
		bool createBuffer(ID3DBlob* blob);

		void createPositionDesc();
		void createColorDesc();
		void createTextureDesc();
		void createNormalDesc();
		void createTangentDesc();
		void create2dDesc();

	private:
		eVertexType m_vertexType;
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_descs;
		ID3D11InputLayout* m_buffer;
	};
}