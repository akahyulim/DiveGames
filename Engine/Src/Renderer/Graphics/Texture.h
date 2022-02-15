#pragma once

namespace Dive
{
	// texture interface
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual bool operator==(const Texture& other) = 0;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(unsigned int width, unsigned int height);
		Texture2D(const std::string& path);
		~Texture2D();

		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView; }

		unsigned int GetWidth() const override { return m_Width; }
		unsigned int GetHeight() const override { return m_Height; }

		bool operator==(const Texture& other) override;

		static Texture2D* Create(unsigned int width, unsigned int height);
		static Texture2D* Create(const std::string& path);

	public:
		void createResource(const BYTE* pData);

	private:
		std::string m_Path;
		bool m_bLoaded = false;
		unsigned int m_Width = 0;
		unsigned int m_Height = 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		bool m_bMipmaps = false; 
		unsigned int m_MipLevels = 0;
		std::vector<std::byte> m_Data;

		ID3D11Texture2D* m_pResource = nullptr;
		ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
	};
}