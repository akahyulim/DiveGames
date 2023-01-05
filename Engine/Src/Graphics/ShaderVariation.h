#pragma once
#include "Core/Object.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Graphics;
	class ConstantBuffer;
	class Shader;

	struct ShaderParameter
	{
		ShaderParameter()
			: Type(eShaderType::None),
			Name(""),
			Offset(0),
			Size(0),
			Index(0),
			pBuffer(nullptr)
		{}

		ShaderParameter(eShaderType type, const std::string& name, uint32_t offset, uint32_t size, uint32_t index)
			: Type(type), 
			Name(name), 
			Offset(offset), 
			Size(size), 
			Index(index),
			pBuffer(nullptr)
		{}

		eShaderType Type;
		std::string Name;
		uint32_t Offset;
		uint32_t Size;
		uint32_t Index;		// constant buffer index
		ConstantBuffer* pBuffer;
	};

	class ShaderVariation
	{
	public:
		explicit ShaderVariation(Shader* pOwner, eShaderType type);
		~ShaderVariation();

		bool CompileAndCreate();
		bool Compile();
		bool CreateVertexShader();
		bool CreatePixelShader();

		ID3D10Blob* GetCompileBuffer() { return m_pCompileBuffer; }

		void* GetShader();

		eShaderType GetShaderType() const { return m_Type; }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		std::string GetDefines() const { return m_Defines; }
		void SetDefines(const std::string& defines) { m_Defines = defines; }

		bool HasParameter(const std::string& name) const;
		const std::unordered_map<std::string, ShaderParameter>& GetParameters() const { return m_Parameters; }

		bool HasTextureUnit(eTextureUnit unit) const { return m_bUseTextureUnits[static_cast<uint32_t>(unit)]; }

		uint64_t GetSemanticsHash() const { return m_SemanticsHash; }

		const unsigned* GetConstantBufferSizes() const { return &m_ConstantBufferSizes[0]; }

	private:
		void parseParameters();
		void calculateConstantBufferSizes();

	private:
		Graphics* m_pGraphics;
		Shader* m_pOwner;
		eShaderType m_Type;
		std::string m_Name;
		ID3D10Blob* m_pCompileBuffer;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		std::string m_Defines;

		uint64_t m_SemanticsHash;

		bool m_bUseTextureUnits[static_cast<uint32_t>(eTextureUnit::Max)];
		
		std::unordered_map<std::string, ShaderParameter> m_Parameters;

		uint32_t m_ConstantBufferSizes[7];
	};
}
