#pragma once
#include "Resource/Resource.h"


namespace Dive
{
	class Dive_Texture : public Resource
	{
	public:
		Dive_Texture(Context* context);
		~Dive_Texture();

		bool SaveToFile(const std::string& filepath) override;
		bool LoadFromFile(const std::string& filepath) override;

		bool CreateRenderTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name = "");
		bool CreateDepthStencilTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, bool readOnly = false, std::string name = "");
		bool CreateCubeTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name = "");
		// LoadFile의 mipmap을 생성하려면 랩핑 함수를 만들어야 한다.
		// 그런데 mipmap을 만든다는 것 자체가 외부 파일 로딩할 때 아닌가?
		// 일단 ImageImporter로 넘기자.
		// => 흐음.. 좀 더 생각해봐야 할듯
		// 예를 들면 수면, 거울 등의 텍스쳐를 만들 때 RenderTargetView로 그리고 mipmap으로 적용하지 않으려나...
		// 그렇다면 위의 것들 전부가 적용 여부를 받아야 하는데...

	private:

	private:
		ID3D11Device* m_device;
		ID3D11ShaderResourceView* m_shaderResourceView;
		ID3D11DepthStencilView* m_depthStencilView;
		ID3D11RenderTargetView* m_renderTargetView;

		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_arraySize;
		unsigned int m_mipLevels;
		unsigned int m_bindFlags;

		DXGI_FORMAT m_format;

	};
}