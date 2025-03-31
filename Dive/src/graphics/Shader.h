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

	namespace ShaderUtils 
	{
		static Microsoft::WRL::ComPtr<ID3DBlob> CompileShaderFile(const std::filesystem::path& filepath, eShaderType type);
		static std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements(eInputLayout layout);
	}

	// Resource 상속을 제거했다.
	class Shader
	{
	public:
		Shader() = default;
		~Shader();

		bool Create(Microsoft::WRL::ComPtr<ID3DBlob> compiledShader, eShaderType type);
		bool CreateInputLayout(Microsoft::WRL::ComPtr<ID3DBlob> blob, eInputLayout layout);
		
		ID3D11VertexShader* GetVertexShader() const { return m_VertexShader; }
		ID3D11HullShader* GetHullShader() const { return m_HullShader; }
		ID3D11DomainShader* GetDomainShader() const { return m_DomainShader; }
		ID3D11GeometryShader* GetGeometryShader() const { return m_GeometryShader; }
		ID3D11ComputeShader* GetComputeShader() const { return m_ComputeShader; }
		ID3D11PixelShader* GetPixelShader() const { return m_PixelShader; }
		ID3D11InputLayout* GetInputLayout() const { return m_InputLayout; }
		
		eShaderType GetType() const { return m_Type; }

		static std::shared_ptr<Shader> Generate(const std::filesystem::path& filepath, eShaderType type, eInputLayout layout = eInputLayout::None);

	private:
		eShaderType m_Type = eShaderType::Undefined;

		ID3D11VertexShader* m_VertexShader = nullptr;
		ID3D11HullShader* m_HullShader = nullptr;
		ID3D11DomainShader* m_DomainShader = nullptr;
		ID3D11GeometryShader* m_GeometryShader = nullptr;
		ID3D11ComputeShader* m_ComputeShader = nullptr;
		ID3D11PixelShader* m_PixelShader = nullptr;
		ID3D11InputLayout* m_InputLayout = nullptr;
	};
}