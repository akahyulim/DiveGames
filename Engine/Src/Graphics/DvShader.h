#pragma once

namespace Dive
{
	enum class eVertexLayout
	{
		Pos_Tex,
		Pos_Tex_Nor,
		Static_Model,
		Skinned_Model
	};

	class DvShader
	{
	public:
		DvShader(const std::string& name, const std::wstring& filepath, eVertexLayout layout = eVertexLayout::Static_Model);
		~DvShader();

		ID3D11VertexShader* GetVertexShader() const { return m_pVertexShader; }
		ID3D11PixelShader* GetPixelShader() const { return m_pPixelShader; }

		ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }

		ID3DBlob* GetVertexShaderBlob() const { return m_pVertexShaderBlob; }

	private:
		void loadFromFile(const std::wstring& filepath);
		void createInputLayout();

	public:
		static DvShader* CreateFromFile(const std::string& name, const std::wstring& filepath, eVertexLayout layout = eVertexLayout::Static_Model);

	private:
	private:
		std::string m_Name;
		std::wstring m_FilePath;

		ID3DBlob* m_pVertexShaderBlob;

		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		eVertexLayout m_VertexLayout;

		ID3D11InputLayout* m_pInputLayout;
	};
}