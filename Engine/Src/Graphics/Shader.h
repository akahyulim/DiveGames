#pragma once
#include "Resource/Resource.h"
#include "GraphicsDefs.h"

namespace Dive
{
	enum class eInputLayout
	{
		None,
		Pos,
		Pos_Tex,
		Pos_Tex_Nor,
		Static_Model,
		Skinned_Model
	};

	// urho3d�� ��� ���� ���̴��� shader variation, Ư�� ���Ϸ� ������ ���̴����� ������ shader�� Ī�ߴ�.
	// �׸��� shader�� ���ҽ��� ����Ѵ�.
	class Shader : public Resource
	{
		DV_CLASS(Shader, Resource);

	public:
		Shader();
		~Shader() override;

		bool CompileAndCreateShader(const std::string& filename, eShaderType type, eInputLayout layout = eInputLayout::None);
		bool CreateInputLayout(eInputLayout layout = eInputLayout::Pos);	// �⺻�� NULL�� �ؾ��ϳ�..

		ID3D11VertexShader* GetVertexShader() const { return m_pVertexShader; }
		ID3D11HullShader* GetHullShader() const { return m_pHullShader; }
		ID3D11DomainShader* GetDomainShader() const { return m_pDomainShader; }
		ID3D11ComputeShader* GetComputeShader() const { return m_pComputeShader; }
		ID3D11PixelShader* GetPixelShader() const { return m_pPixelShader; }
		ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }
		ID3DBlob* GetVertexShaderBlob() const { return m_pVertexShaderBlob; }

		eShaderType GetType() const { return m_Type; }

		void SetShader();

	private:
		bool createInputLayout(ID3D10Blob* pShaderBlob, eInputLayout layout);

	private:
		std::string m_FilePath;		// Resource�� sring�̴�..

		eShaderType m_Type;

		ID3DBlob* m_pVertexShaderBlob;

		ID3D11VertexShader* m_pVertexShader;
		ID3D11HullShader* m_pHullShader;
		ID3D11DomainShader* m_pDomainShader;
		ID3D11ComputeShader* m_pComputeShader;
		ID3D11PixelShader* m_pPixelShader;
		
		ID3D11InputLayout* m_pInputLayout;
	};
}