#pragma once
#include "Core/Object.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Graphics;

	class Shader : public Object
	{
		DIVE_OBJECT(Shader, Object)
	
	public:
		explicit Shader(Context* pContext, eShaderType type);
		~Shader() override;

		// 컴파일 후 생성.
		bool Compile(const std::string& filepath);
		// 릴리즈.
		void Release();

		// 컴파일 바이트 코드 리턴.
		const std::vector<unsigned char>& GetByteCode() const { return m_ByteCode; }
		// 셰이더 리턴.
		void* GetShader();

		// 셰이더 타입 리턴.
		eShaderType GetShaderType() const { return m_Type; }

		// 셰이더 이름 리턴.
		const std::string& GetName() const { return m_Name; }
		// 셰이더 이름 설정.
		void SetName(const std::string& name) { m_Name = name; }

		// 매크로 정의 추가.
		void AddDefine(const std::string& define, const std::string& value = "1") { m_Defines[define] = value; }
		// 매크로 정의 맵 리턴.
		const std::unordered_map<std::string, std::string>& GetDefines() const { return m_Defines; }

	private:

	private:
		// 그래픽스 장치 객체 포인터.
		Graphics* m_pGraphics;
		// 셰이더 타입.
		eShaderType m_Type;
		// 셰이더 이름.
		std::string m_Name;
		// 컴파일 바이트 코드.
		std::vector<unsigned char> m_ByteCode;
		// 정점 셰이더.
		ID3D11VertexShader* m_pVertexShader;
		// 픽셀 셰이더.
		ID3D11PixelShader* m_pPixelShader;
		// 매크로 정의 맵.
		std::unordered_map<std::string, std::string> m_Defines;
	};
}
