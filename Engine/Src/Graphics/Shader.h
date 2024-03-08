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

	// urho3d�� ��� ���� ���̴��� shader variation, Ư�� ���Ϸ� ������ ���̴����� ������ shader�� Ī�ߴ�.
	// �׸��� shader�� ���ҽ��� ����Ѵ�.
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
		std::string m_FilePath;		// Resource�� sring�̴�..

		ID3DBlob* m_pVertexShaderBlob;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		ID3D11InputLayout* m_pInputLayout;
	};

	Shader* LoadShaderFromFile(const std::string& fileName);
	Shader* CreateShader(const std::string& name);
}