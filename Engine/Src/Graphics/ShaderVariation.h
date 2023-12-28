#pragma once
#include "Core/Object.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class InputLayout;

	// 굳이 Object를 상속해야 하나?
	// Name은 사용할 수 있지만 ID는 사용하지 않는다.
	class ShaderVariation : public Object
	{
	public:
		ShaderVariation();
		~ShaderVariation();

		bool CompileAndCreate(eShaderType type, const std::string& filePath, eVertexType vertexType = eVertexType::Undefined);
		bool Compile(eShaderType type, const std::string& filePath);
		bool CreateShader(eShaderType type, eVertexType vertexType = eVertexType::Undefined);
		bool CreateInputLayout(eVertexType vertexType);

		std::string GetFilePath() const { return m_FilePath; }
		eShaderType GetShaderType() const { return m_ShaderType; }
		void* GetShaderResource() const { return m_pShaderResource; }
		InputLayout* GetInputLayout() const;

	private:


	private:
		std::string m_FilePath;

		eShaderType m_ShaderType;

		ID3D10Blob* m_pShaderBuffer;

		void* m_pShaderResource;
	
		InputLayout* m_pInputLayout;
	};
}