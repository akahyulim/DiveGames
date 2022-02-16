#include "divepch.h"
#include "Texture.h"
#include "Base/Base.h"
#include "Dive.h"

namespace Dive
{
	unsigned int GetChannelCount(DXGI_FORMAT format)
	{
		unsigned int channelCount = 0;

		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			channelCount = 4;
			break;
		default:
			break;
		}

		return channelCount;
	}

	// 이건 RenderTargetView + ShaderResourceView 겠지?
	Texture2D::Texture2D(unsigned int width, unsigned int height)
		: m_Width(width), m_Height(height)
	{
	}
	
	// 현재 RenderTargetView는 생성하지 않는다.
	Texture2D::Texture2D(const std::string& path)
		: m_Path(path)
	{
		DirectX::ScratchImage img;

		std::wstring tempPath(path.begin(), path.end());
		WCHAR ext[_MAX_EXT];
		_wsplitpath_s(tempPath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

		HRESULT hResult = 0;

		// 애매해...
		hResult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

		if (_wcsicmp(ext, L".dds") == 0)
		{
			hResult = DirectX::LoadFromDDSFile(tempPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (_wcsicmp(ext, L".tga") == 0)
		{
			hResult = DirectX::LoadFromTGAFile(tempPath.c_str(), nullptr, img);
		}
		else
		{
			hResult = DirectX::LoadFromWICFile(tempPath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, img);
		}

		if(FAILED(hResult))
		{
			DV_CORE_WARN("{:s} 로드에 실패하였습니다.", m_Path);
			return;
		}

		auto pDevice = Dive::GetRenderer().GetGraphicsDevice()->GetDevice();
		DV_ASSERT(pDevice);

		DirectX::CreateShaderResourceView(pDevice, img.GetImages(), img.GetImageCount(), img.GetMetadata(), &m_pShaderResourceView);
	}

	Texture2D::~Texture2D()
	{
	}
	
	bool Texture2D::operator==(const Texture& other)
	{
		return false;
	}

	Texture2D* Texture2D::Create(unsigned int width, unsigned int height)
	{
		return new Texture2D(width, height);
	}

	Texture2D* Texture2D::Create(const std::string& path)
	{
		return new Texture2D(path);
	}
}