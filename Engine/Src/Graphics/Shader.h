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

		// ������ �� ����.
		bool Compile(const std::string& filepath);
		// ������.
		void Release();

		// ������ ����Ʈ �ڵ� ����.
		const std::vector<unsigned char>& GetByteCode() const { return m_ByteCode; }
		// ���̴� ����.
		void* GetShader();

		// ���̴� Ÿ�� ����.
		eShaderType GetShaderType() const { return m_Type; }

		// ���̴� �̸� ����.
		const std::string& GetName() const { return m_Name; }
		// ���̴� �̸� ����.
		void SetName(const std::string& name) { m_Name = name; }

		// ��ũ�� ���� �߰�.
		void AddDefine(const std::string& define, const std::string& value = "1") { m_Defines[define] = value; }
		// ��ũ�� ���� �� ����.
		const std::unordered_map<std::string, std::string>& GetDefines() const { return m_Defines; }

	private:

	private:
		// �׷��Ƚ� ��ġ ��ü ������.
		Graphics* m_pGraphics;
		// ���̴� Ÿ��.
		eShaderType m_Type;
		// ���̴� �̸�.
		std::string m_Name;
		// ������ ����Ʈ �ڵ�.
		std::vector<unsigned char> m_ByteCode;
		// ���� ���̴�.
		ID3D11VertexShader* m_pVertexShader;
		// �ȼ� ���̴�.
		ID3D11PixelShader* m_pPixelShader;
		// ��ũ�� ���� ��.
		std::unordered_map<std::string, std::string> m_Defines;
	};
}
