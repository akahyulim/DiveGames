#include "dvTexture.h"
#include "DiveCore.h"
#include "Log.h"
#include "FileSystemHelper.h"

// ������ ������ ���� Resource���� �����ؾ� �Ѵ�.
// ���� string <-> wstring ��ȯ �Լ��� ������.
namespace dive
{
	dvTexture::dvTexture(ID3D11Device* pDevice)
	{
		DV_ASSERT(pDevice != nullptr);
		m_pDevice = pDevice;
	}

	bool dvTexture::LoadFromFile(const std::string& filepath, bool generateMips)
	{
		// string <-> wstring �Լ� �ʿ�

		return true;
	}
}