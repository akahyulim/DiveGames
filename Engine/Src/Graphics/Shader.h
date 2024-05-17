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

	// urho3d의 경우 개별 셰이더는 shader variation, 특정 파일로 생성된 셰이더들의 묶음을 shader라 칭했다.
	// 그리고 shader만 리소스를 상속한다.
	class Shader : public Resource
	{
		DV_CLASS(Shader, Resource);

	public:
		Shader();
		~Shader() override;

		bool CompileAndCreateShader(const std::string& filename, eShaderType type, eInputLayout layout = eInputLayout::None);
		bool CreateInputLayout(eInputLayout layout = eInputLayout::Pos);	// 기본을 NULL로 해야하나..

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
		std::string m_FilePath;		// Resource는 sring이다..

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