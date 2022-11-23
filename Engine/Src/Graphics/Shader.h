#pragma once
#include "Core/Object.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Graphics;
	class ConstantBuffer;

	// 현재 셰이더에서 사용하는 ConstantBuffer의
	// 변수 이름, offset, 타입으로 유추한 size
	// 그리고 ConstantBuffer의 index와 연결된 버퍼를 관리한다.
	// 즉, ConstantBuffer의 구성 변수만큼 존재하게 된다.
	// 결국은 d3d9처럼 개별 파라미터 변수로 관리하는 것이다.
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

	// 현재 리소스가 아니다.
	class Shader : public Object
	{
		DIVE_OBJECT(Shader, Object)
	
	public:
		explicit Shader(Context* pContext, eShaderType type);
		~Shader() override;

		bool Compile(const std::string& filepath);
		
		ID3D10Blob* GetShaderBuffer() { return m_pShaderBuffer; }
		void* GetShader();

		eShaderType GetShaderType() const { return m_Type; }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		const std::unordered_map<std::string, std::string>& GetDefines() const { return m_Defines; }
		void AddDefine(const std::string& define, const std::string& value = "1") { m_Defines[define] = value; }
		
		//std::string GetDefines() const { return m_Defines; }
		//void SetDefines(const std::string& defines) { m_Defines = defines; }

		bool HasParameter(const std::string& name) const;
		const std::unordered_map<std::string, ShaderParameter>& GetParameters() const { return m_Parameters; }

		const unsigned* GetConstantBufferSizes() const { return &m_ConstantBufferSizes[0]; }

	private:
		void parseParameters();
		void calculateConstantBufferSizes();

	private:
		Graphics* m_pGraphics = nullptr;
		eShaderType m_Type = eShaderType::None;
		std::string m_Name;
		//Microsoft::WRL::ComPtr<ID3D10Blob> m_pShaderBuffer;
		ID3D10Blob* m_pShaderBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
		std::unordered_map<std::string, std::string> m_Defines;
		//std::string m_Defines;

		bool m_bUseTextureUnits[16] = { false, };
		
		std::unordered_map<std::string, ShaderParameter> m_Parameters;

		unsigned int m_ConstantBufferSizes[7] = { 0, };
	};
}
