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
		ShaderParameter() = default;
		ShaderParameter(eShaderType type, const std::string& name, unsigned int offset, unsigned int size, unsigned int index)
			: m_Type(type), m_Name(name), m_Offset(offset), m_Size(size), m_Index(index)
		{}

		eShaderType m_Type = eShaderType::None;
		std::string m_Name;
		unsigned int m_Offset = 0;
		unsigned int m_Size = 0;
		unsigned int m_Index = 0;		// constant buffer index
		ConstantBuffer* m_pBuffer = nullptr;
	};

	class ShaderVariation
	{
	public:
		explicit ShaderVariation(Shader* pOwner, eShaderType type);
		~ShaderVariation();

		bool Compile(const std::string& filepath);
		bool Create();

		// 이름이 헷갈릴 수 있다.
		ID3D10Blob* GetShaderBuffer() { return m_pShaderBuffer; }
		void* GetShader();

		eShaderType GetShaderType() const { return m_Type; }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		const std::unordered_map<std::string, std::string>& GetDefines() const { return m_Defines; }
		void AddDefine(const std::string& define, const std::string& value = "1") { m_Defines[define] = value; }

		//std::string GetDefines() const { return m_Defines; }
		void SetDefines(const std::string& defines) {}//{ m_Defines = defines; }

		bool HasParameter(const std::string& name) const;
		const std::unordered_map<std::string, ShaderParameter>& GetParameters() const { return m_Parameters; }

		unsigned long long GetSemanticsHash() const { return m_SemanticsHash; }

		const unsigned* GetConstantBufferSizes() const { return &m_ConstantBufferSizes[0]; }

	private:
		void parseParameters();
		void calculateConstantBufferSizes();

	private:
		Shader* m_pOwner = nullptr;
		Graphics* m_pGraphics = nullptr;
		eShaderType m_Type = eShaderType::None;
		std::string m_Name;
		//Microsoft::WRL::ComPtr<ID3D10Blob> m_pShaderBuffer;
		ID3D10Blob* m_pShaderBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
		std::unordered_map<std::string, std::string> m_Defines;
		//std::string m_Defines;

		unsigned long long m_SemanticsHash = 0;

		bool m_bUseTextureUnits[16] = { false, };
		
		std::unordered_map<std::string, ShaderParameter> m_Parameters;

		unsigned int m_ConstantBufferSizes[7] = { 0, };
	};
}
