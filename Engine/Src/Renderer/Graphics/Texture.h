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

	// 일단 밉맵과 배열은 제외
	class Texture2D : public Texture
	{
	public:
		Texture2D() = default;
		~Texture2D();

		// 일단은 넘어가지만
		// 여기도 ShaderResourceView가 추가될 수 있다.
		bool LoadEngineFile(const std::string& path);
		bool LoadForeignFile(const std::string& path);

		bool CreateTexture2D(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned bindFlags);
		bool CreateShaderResourceView(DXGI_FORMAT format);
		bool CreateRenderTargetView(DXGI_FORMAT format);
		bool CreateDepthStencilView(DXGI_FORMAT format);

		void Shutdown();

		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView; }
		ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() { return m_pDepthStencilView; }

		D3D11_VIEWPORT* GetViewport() { return &m_Viewport; }

		bool operator==(const Texture& other) override;

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		std::string GetPath() const { return m_Path; }

		unsigned int GetWidth() const override { return m_Width; }
		unsigned int GetHeight() const override { return m_Height; }

		// RenderTargetView + ShaderResourceView
		static Texture2D* Create(unsigned int width, unsigned int height, DXGI_FORMAT format, bool srv = false, const std::string& name = "");
		// DepthStencilView + ShaderResourceView 
		static Texture2D* Create(unsigned int width, unsigned int height, DXGI_FORMAT texture2D, DXGI_FORMAT dsv, DXGI_FORMAT srv = DXGI_FORMAT_UNKNOWN, const std::string& name = "");
		// ShaderResourceView
		static Texture2D* Create(const std::string& path, const std::string& name = "");

	private:
		std::string m_Name;
		std::string m_Path;
		unsigned int m_Width	= 0;
		unsigned int m_Height	= 0;
		DXGI_FORMAT m_Format	= DXGI_FORMAT_UNKNOWN;
		
		ID3D11Texture2D* m_pTexture2D					= nullptr;
		ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
		ID3D11RenderTargetView* m_pRenderTargetView		= nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView		= nullptr;

		D3D11_VIEWPORT m_Viewport;
	};
}