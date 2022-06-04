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
	
	// 1. Cube를 만들때처럼 texture array를 생성할 수 있을까?
	// 2. 스파르탄처럼 img에서 필요한 데이터만 뽑아 낼 수 있을까?
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
			DV_CORE_ERROR("{:s} 로드에 실패하였습니다.", filepath);
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
			DV_CORE_ERROR("ID3D11Textur2D 생성에 실패하였습니다.");
			return false;
		}

		// 기본 데이터까진 Set하는 편이 나아 보인다.

		// 문제는 srv, rtv 등도 이 곳에서 생성할 것인가 이다.

		// 스파르탄의 경우
		// 1. bindflags만 추가한 texture 생성
		// 2. 생성한 texture의 LoadFromFile호출
		// 3. LoadFromFile 내부에서 Importer의 Load에 경로와 자신의 포인터를 전달하여 로드한 데이터 저장
		// 4. 다시 Texture::LoadFromFile로 돌아와 CreateResource()를 호출하여 Resource와 View를 생성

		img.Release();

		return true;
	}
}