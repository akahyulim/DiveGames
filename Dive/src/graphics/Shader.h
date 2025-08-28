#pragma once
#include "core/Object.h"

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// enum class
	//-------------------------------------------------------------------------------------
	enum class eInputLayout : uint8_t
	{
		None = 0,
		PN,
		PNT,
		Unlit,
		Lit,
		Skinned
	};

	class IShader : public Object
	{
	public:
		virtual ~IShader() = default;
		virtual bool LoadFromFile(const std::filesystem::path& path) = 0;
		virtual void Bind() = 0;
	};

	class VertexShader : public IShader
	{
	public:
		~VertexShader() override;
		bool LoadFromFile(const std::filesystem::path& path) override;
		void Bind() override;

		ID3D11VertexShader* GetShader() const { return m_shader.Get(); }
		ID3DBlob* GetBytecode() const { return m_blob.Get(); };

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
	};

	class PixelShader : public IShader
	{
	public:
		~PixelShader() override;
		bool LoadFromFile(const std::filesystem::path& path) override;
		void Bind() override;

		ID3D11PixelShader* GetShader() const { return m_shader.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shader;
	};
}
