#pragma once
#include "Texture.h"

// 1. createTextureResource()�ȿ��� �ٽ� �Լ��� ȣ���ϴ°� ���ٴ�.
// 2. ���� ���ڿ� ���� ���ص��� ���� �� �ٵ��� �ʿ䰡 �ִ�.


namespace Dive
{
	class Context;

	// ���ĸ�ź�� Texture�� viewport�� ��������� �����ϸ�,
	// ��� �����Լ����� ũ�⿡ ���� �ʱ�ȭ�Ѵ�.
	class Texture2D : public Texture
	{
	public:
		// sharder resource?
		Texture2D(Context* context, unsigned int width, unsigned int height, DXGI_FORMAT format, const std::vector<std::vector<std::byte>>& data);

		
		// format�� �������� RenderTargetView & ShaderResourceView Ȥ�� DepthStencilView & ShaderResourceView�� �����Ѵ�.	
		Texture2D(Context* context, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int arraySize, unsigned int flags, std::string name = " ");

	private:
		bool createTextureResource() override;
	};
}