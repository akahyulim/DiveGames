#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// enum class
	//-------------------------------------------------------------------------------------
	enum class eShaderType : uint8_t
	{
		Undefined = 0,
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Compute
	};

	enum class eInputLayout : uint8_t
	{
		None = 0,
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
		DV_CLASS(Shader, Resource)

	public:
		Shader();
		~Shader() override;

		bool CompileAndCreateShader(const std::string& filename, eShaderType type, eInputLayout layout = eInputLayout::None);
		
		ID3D11VertexShader* GetVertexShader() const { return m_pVertexShader; }
		ID3D11HullShader* GetHullShader() const { return m_pHullShader; }
		ID3D11DomainShader* GetDomainShader() const { return m_pDomainShader; }
		ID3D11GeometryShader* GetGeometryShader() const { return m_pGeometryShader; }
		ID3D11ComputeShader* GetComputeShader() const { return m_pComputeShader; }
		ID3D11PixelShader* GetPixelShader() const { return m_pPixelShader; }
		ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }
		
		eShaderType GetType() const { return m_Type; }

	private:
		bool createInputLayout(ID3D10Blob* pShaderBlob, eInputLayout layout);

	private:
		std::string m_FilePath;		// Resource�� sring�̴�..

		eShaderType m_Type;

		ID3D11VertexShader* m_pVertexShader;
		ID3D11HullShader* m_pHullShader;
		ID3D11DomainShader* m_pDomainShader;
		ID3D11GeometryShader* m_pGeometryShader;
		ID3D11ComputeShader* m_pComputeShader;
		ID3D11PixelShader* m_pPixelShader;
		
		ID3D11InputLayout* m_pInputLayout;
	};
}