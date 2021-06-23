#include "Model.h"
#include "dvMesh.h"
#include "Renderer.h"
#include "Log.h"

namespace dive
{
	Model::Model()
	{
		m_pMesh = nullptr;
	}

	Model::~Model()
	{
	}
	
	void Model::Clear()
	{
		// 버퍼를 제거한다.
	}
	
	void Model::SetMesh(dvMesh* pMesh)
	{
		// 일단 현재 메시와 비교
		// 같다면 리턴
		// 다르다면 clear 후 저장
		// 그리고 버퍼 생성
	}

	bool Model::createBuffers()
	{
		// 요소별로 나누어서 버퍼를 생성한다.
		// 몇몇 요소는 없을 경우 직접 계산하여 생성한다.

		return false;
	}
}