#pragma once
#include "Core/Object.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Graphics;

	// ���� ���ҽ��� �ƴϴ�.
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

		void AddDefine(const std::string& define, const std::string& value = "1") { m_Defines[define] = value; }
		const std::unordered_map<std::string, std::string>& GetDefines() const { return m_Defines; }

	private:
		Graphics* m_pGraphics;
		eShaderType m_Type;
		std::string m_Name;
		//Microsoft::WRL::ComPtr<ID3D10Blob> m_pShaderBuffer;
		ID3D10Blob* m_pShaderBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
		std::unordered_map<std::string, std::string> m_Defines;
	};
}
