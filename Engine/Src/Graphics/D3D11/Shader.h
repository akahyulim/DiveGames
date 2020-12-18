#pragma once
#include "Core/Object.h"
#include "VertexTypes.h"
#include "InputLayout.h"


namespace Dive
{ 
	class Context;

	enum class eShaderType
	{
		Vertex,
		Pixel,
		VertexPixel,
	};

	// 일단 이것부터 완성한 후
	// 삼각형 출력을 향해 가자.
	class Shader : public Object
	{
		DIVE_OBJECT(Shader, Object);

	public:
		Shader(Context* context);
		~Shader();

		// compile 후 shader와 input layout 생성
		// input layout 생성을 위해선 vertex type 필요
		
		ID3D11VertexShader* GetVertexShader()	const { return m_vertexShader; }
		ID3D11PixelShader* GetPixelShader()		const { return m_pixelShader; }
		ID3D11InputLayout* GetInputLayout()		const { return m_inputLayout ? m_inputLayout->GetBuffer() : nullptr; }

		eShaderType GetType()					const { return m_type; }

	private:


	private:
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		std::shared_ptr<InputLayout> m_inputLayout;
		
		eShaderType m_type;

		std::string m_name;
		std::string m_filepath;
	};
}