#include "dvTexture.h"
#include "DiveCore.h"
#include "Log.h"
#include "FileSystemHelper.h"

// 꼬리에 꼬리를 물려 Resource부터 구현해야 한다.
// 이후 string <-> wstring 변환 함수를 만들자.
namespace dive
{
	dvTexture::dvTexture(ID3D11Device* pDevice)
	{
		DV_ASSERT(pDevice != nullptr);
		m_pDevice = pDevice;
	}

	bool dvTexture::LoadFromFile(const std::string& filepath, bool generateMips)
	{
		// string <-> wstring 함수 필요

		return true;
	}
}