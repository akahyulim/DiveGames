#pragma once
#include "dvTexture.h"
#include <string>
#include <DirectXMath.h>

namespace dive
{
	class dvTexture2D : public dvTexture
	{
	public:
		// �ᱹ � �ؽ��ĸ� �����ϰ�, �ű⿣ � ������ �ʿ������� �߿��ϴ�.

		dvTexture2D(const std::string& name = "")
			: dvTexture(typeid(dvTexture2D).hash_code())  
		{
			SetName(name);
		}

		bool CreateColorRenderTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int mipCount = 1);

		bool CreateDepthRenderTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, bool readOnly = false, unsigned int mipCount = 1);

		bool LoadFromFile(const std::string& filepath, bool createMips = false);
		bool SaveToFile(const std::string& filepath);

		void Clear();


		// static �Լ��� Ư�� ������ �ؽ��ĸ� ���� = ����Ƽ�� �־��� �� ����.

	private:

	private:
	};
}