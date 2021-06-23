#pragma once
#include "GraphicsInclude.h"
#include "DirectXTex/DirectXTex.h"
#include <string>
#include <vector>

namespace dive
{
	// 1. 일단 2D Texture 전용
	// 2. DepthStencilView는 미구현(Deferred에서 다시 해보자.)
	// 3. 파일 관련 Helper 함수 필요

	class Texture
	{
	public:
		// cpu에서 그리기용(SRV)
		Texture(unsigned int width, unsigned int height);

		// SRV(파일명, mipmap 생성 여부)
		Texture(const std::wstring& filepath, bool generateMips = true);

		// RTV or DSV(size, view type, format type?)
		Texture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name = "");


		ID3D11Texture2D* GetTexture2D() { return mTexture2D.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() { return mRenderTargetView.Get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() { return mShaderResourceView.Get(); }

		// get & set
		unsigned int GetWidth() const { return mWidth; }
		unsigned int GetHeight() const { return mHeight; }
		unsigned int GetMipLevls() const { return mMipLevels; }
		unsigned int GetArraySize() const { return mArraySize; }
		DXGI_FORMAT GetFormat() const { return mFormat; }
		std::string GetName() const { return mName; }

		void SetWidth(unsigned int width) { mWidth = width; }
		void SetHeight(unsigned int height) { mHeight = height; }
		void SetFormat(DXGI_FORMAT format) { mFormat = format; }

		void SetChannelCount(unsigned int count) { mChannelCount = count; }
		void SetBitsPerChannel(unsigned int value) { mBitsPerChannel = value; }
		void SetPitch(unsigned int pitch) { mPitch = pitch; }
		void SetData(std::vector<std::byte>& data) { mData = data; }


	private:
		bool createTexture2D(ID3D11Device* device, unsigned flags);
		bool createShaderResourceView(ID3D11Device* device);
		bool createRenderTargetView(ID3D11Device* device);
		bool createDepthStencilView(ID3D11Device* device);

		void setMetaData(const DirectX::TexMetadata& data);

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture2D;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;

		std::string mName;

		// 최소한의 데이터 필요
		// 필요없는 건 지우자.
		unsigned int mWidth = 0;
		unsigned int mHeight = 0;
		unsigned int mMipLevels = 0;
		unsigned int mArraySize = 0;
		unsigned int mChannelCount = 0;
		unsigned int mBitsPerChannel = 0;
		unsigned int mPitch = 0;
		DXGI_FORMAT mFormat = DXGI_FORMAT_UNKNOWN;
		std::vector<std::byte> mData;
	};
}