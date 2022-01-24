#pragma once
#include "GraphicsInclude.h"
#include "DirectXTex/DirectXTex.h"
#include <string>
#include <vector>

namespace Dive
{
	// 1. �ϴ� 2D Texture ����
	// 2. DepthStencilView�� �̱���(Deferred���� �ٽ� �غ���.)
	// 3. ���� ���� Helper �Լ� �ʿ�

	class Texture
	{
	public:
		Texture(ID3D11Device* pDevice);

		// cpu���� �׸����(SRV)
		Texture(unsigned int width, unsigned int height);

		// SRV(���ϸ�, mipmap ���� ����) => �⺻ �����ڷ� �����ϱ�
		// �׸��� LoadFromFile()�� ��ü
		Texture(const std::wstring& filepath, bool generateMips = true);

		// RTV or DSV(size, view type, format type?)
		Texture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name = "");

		bool LoadFromFile(const std::wstring& filepath, bool generateMips = false);

		ID3D11Texture2D* GetTexture2D() { return m_pTexture2D.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView.Get(); }

		// get & set
		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }
		unsigned int GetMipLevls() const { return m_MipLevels; }
		unsigned int GetArraySize() const { return m_ArraySize; }
		DXGI_FORMAT GetFormat() const { return m_Format; }
		std::string GetName() const { return m_Name; }

		void SetWidth(unsigned int width) { m_Width = width; }
		void SetHeight(unsigned int height) { m_Height = height; }
		void SetFormat(DXGI_FORMAT format) { m_Format = format; }

		void SetChannelCount(unsigned int count) { m_ChannelCount = count; }
		void SetBitsPerChannel(unsigned int value) { m_BitsPerChannel = value; }
		void SetPitch(unsigned int pitch) { m_Pitch = pitch; }
		void SetData(std::vector<std::byte>& data) { m_Data = data; }


	private:
		bool createTexture2D(ID3D11Device* pDevice, unsigned flags);
		bool createShaderResourceView(ID3D11Device* pDevice);
		bool createRenderTargetView(ID3D11Device* pDevice);
		bool createDepthStencilView(ID3D11Device* pDevice);

		void setMetaData(const DirectX::TexMetadata& data);

	private:
		ID3D11Device* m_pDevice = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture2D;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		
		std::string m_Name = "";

		// �ּ����� ������ �ʿ�
		// �ʿ���� �� ������.
		unsigned int m_Width = 0;
		unsigned int m_Height = 0;
		unsigned int m_MipLevels = 0;
		unsigned int m_ArraySize = 0;
		unsigned int m_ChannelCount = 0;
		unsigned int m_BitsPerChannel = 0;
		unsigned int m_Pitch = 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		std::vector<std::byte> m_Data;
	};
}