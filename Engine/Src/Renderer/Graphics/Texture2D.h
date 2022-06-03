#pragma once
#include "Texture.h"

namespace Dive
{
	// ¹Ó¸Ê°ú ¹è¿­Àº Á¦¿Ü
	class Texture2D : public Texture
	{
	public:
		RESOURCE_CLASS_TYPE(Texture2D)
	public:
		Texture2D();
		Texture2D(unsigned long long id);
		//Texture2D(const std::string& name = "", unsigned long long id = 0);
		~Texture2D();

		bool create() override;

		bool SaveFromFile(const std::string& filepath);
		bool LoadFromFile(const std::string& filepath);

		bool CreateTexture2D(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned bindFlags);
		bool CreateShaderResourceView(DXGI_FORMAT format);
		bool CreateRenderTargetView(DXGI_FORMAT format);
		bool CreateDepthStencilView(DXGI_FORMAT format);

		bool operator==(const Texture& other) override;

		bool SetSize(int width, int height, DXGI_FORMAT format, unsigned int bindFlags);

		// RenderTargetView + ShaderResourceView
		static Texture2D* Create(unsigned int width, unsigned int height, DXGI_FORMAT format, bool srv = false, const std::string& name = "");
		// DepthStencilView + ShaderResourceView 
		static Texture2D* Create(unsigned int width, unsigned int height, DXGI_FORMAT texture2D, DXGI_FORMAT dsv, DXGI_FORMAT srv = DXGI_FORMAT_UNKNOWN, const std::string& name = "");
		// ShaderResourceView
		static Texture2D* Create(const std::string& path, const std::string& name = "");
	};
}