#include "divepch.h"
#include "ImageImporter.h"
#include "Renderer/Renderer.h"
#include "Renderer/Graphics/GraphicsDevice.h"
#include "Renderer/Graphics/Texture2D.h"

namespace Dive
{
	ImageImporter::ImageImporter()
	{
		m_pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(m_pDevice);
	}

	ImageImporter::~ImageImporter()
	{
	}
	
	// 1. Cube�� ���鶧ó�� texture array�� ������ �� ������?
	// 2. ���ĸ�źó�� img���� �ʿ��� �����͸� �̾� �� �� ������?
	bool ImageImporter::Load(const std::string& filepath, Texture* pTexture)
	{
		if (!m_pDevice || !pTexture)
			return false;

		DirectX::ScratchImage img;

		std::wstring tempPath(filepath.begin(), filepath.end());
		WCHAR ext[_MAX_EXT];
		_wsplitpath_s(tempPath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

		HRESULT hResult = 0;
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

		if (FAILED(hResult))
		{
			DV_CORE_ERROR("{:s} �ε忡 �����Ͽ����ϴ�.", filepath);
			return false;
		}

		if (FAILED(DirectX::CreateTexture(
			m_pDevice,
			img.GetImages(),
			img.GetImageCount(),
			img.GetMetadata(),
			(ID3D11Resource**)pTexture->GetTexture2D()
		)))
		{
			DV_CORE_ERROR("ID3D11Textur2D ������ �����Ͽ����ϴ�.");
			return false;
		}

		// �⺻ �����ͱ��� Set�ϴ� ���� ���� ���δ�.

		// ������ srv, rtv � �� ������ ������ ���ΰ� �̴�.

		// ���ĸ�ź�� ���
		// 1. bindflags�� �߰��� texture ����
		// 2. ������ texture�� LoadFromFileȣ��
		// 3. LoadFromFile ���ο��� Importer�� Load�� ��ο� �ڽ��� �����͸� �����Ͽ� �ε��� ������ ����
		// 4. �ٽ� Texture::LoadFromFile�� ���ƿ� CreateResource()�� ȣ���Ͽ� Resource�� View�� ����

		img.Release();

		return true;
	}
}