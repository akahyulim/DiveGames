#pragma once
#include "Core/Object.h"
#include "../VertexTypes.h"


namespace Dive
{ 
	class Context;

	enum class eShaderType
	{
		Vertex,
		Pixel,
		VertexPixel,
		Unknown
	};


	class Shader : public Object
	{
		DIVE_OBJECT(Shader, Object);

	public:
		Shader(Context* context);
		~Shader();

		// VertexType을 직접전달하는 것이 아니라 Vertex Buffer를 이용해 유추할 순 없을까?
		bool CreateShader(eVertexType vertexType, std::wstring filepath);
		bool CreateVertexShader(eVertexType vertexType, std::wstring filepath);
		bool CreatePixelShader(std::wstring filepath);
		
		ID3D11VertexShader* GetVertexShader()	const { return m_vertexShader; }
		ID3D11PixelShader* GetPixelShader()		const { return m_pixelShader; }
		ID3D11InputLayout* GetInputLayout()		const { return m_inputLayout; }

		eShaderType GetShaderType()				const { return m_shaderType; }
		eVertexType GetVertexType()				const { return m_vertexType; }

	private:
		bool compileAndCreateShader(std::wstring filepath);
		bool createVertexShader(ID3D10Blob* blob);
		bool createPixelShader(ID3D10Blob* blob);

		bool createInputLayout(ID3D10Blob* blob);
		void createPositionDesc();
		void createColorDesc();
		void createTextureDesc();
		void createNormalDesc();
		void createTangentDesc();

	private:
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		ID3D11InputLayout* m_inputLayout;
		
		eShaderType m_shaderType;
		eVertexType m_vertexType;
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputDescs;

		std::string m_name;
		std::wstring m_filepath;
	};
}