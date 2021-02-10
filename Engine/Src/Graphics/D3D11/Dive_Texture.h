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
		// LoadFile�� mipmap�� �����Ϸ��� ���� �Լ��� ������ �Ѵ�.
		// �׷��� mipmap�� ����ٴ� �� ��ü�� �ܺ� ���� �ε��� �� �ƴѰ�?
		// �ϴ� ImageImporter�� �ѱ���.
		// => ����.. �� �� �����غ��� �ҵ�
		// ���� ��� ����, �ſ� ���� �ؽ��ĸ� ���� �� RenderTargetView�� �׸��� mipmap���� �������� ��������...
		// �׷��ٸ� ���� �͵� ���ΰ� ���� ���θ� �޾ƾ� �ϴµ�...

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