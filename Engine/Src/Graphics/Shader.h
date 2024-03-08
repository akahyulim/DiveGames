#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	enum class eVertexLayout
	{
		Pos_Tex,
		Pos_Tex_Nor,
		Static_Model,
		Skinned_Model
	};

	// urho3d의 경우 개별 셰이더는 shader variation, 특정 파일로 생성된 셰이더들의 묶음을 shader라 칭했다.
	// 그리고 shader만 리소스를 상속한다.
	class Shader : public Resource
	{
		DV_OBJECT(Shader, Resource);

	public:
		Shader();
		~Shader() override;

		bool LoadFromFile(const std::string& fileName) override;
		bool CreateInputLayout(eVertexLayout layout);

		ID3D11VertexShader* GetVertexShader() const { return m_pVertexShader; }
		ID3D11PixelShader* GetPixelShader() const { return m_pPixelShader; }
		ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }
		ID3DBlob* GetVertexShaderBlob() const { return m_pVertexShaderBlob; }

	private:
		std::string m_FilePath;		// Resource는 sring이다..

		ID3DBlob* m_pVertexShaderBlob;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		ID3D11InputLayout* m_pInputLayout;
	};

	Shader* LoadShaderFromFile(const std::string& fileName);
	Shader* CreateShader(const std::string& name);
}