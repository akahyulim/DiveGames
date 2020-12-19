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

	// �ϴ� �̰ͺ��� �ϼ��� ��
	// �ﰢ�� ����� ���� ����.
	class Shader : public Object
	{
		DIVE_OBJECT(Shader, Object);

	public:
		Shader(Context* context);
		~Shader();

		// compile �� shader�� input layout ����
		// input layout ������ ���ؼ� vertex type �ʿ�
		
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