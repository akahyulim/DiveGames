#pragma once
#include "../Resource/Resource.h"
#include <string>

namespace Dive
{
	class dvTexture2D;

	/*
	*	스파르탄의 경우
	*	Resource를 상속한다.
	*	각종 데이터(색상, Texture 경로, 기타)를 XML 파일로 Save, Load한다.
	*	Load의 경우 Texture는 ResourceCache로부터 이름을 전달하여 가져온다.
	*	만약 Load되어 있지 않다면 직접 Load한다.
	*	아직 단위가 단일 Mesh인지 Model인지 모르겠다.
	*	Albedo 파일이 Mesh 단위로 나누어진 경우도 있기 때문이다.
	*/
	class Material : public Resource
	{
	public:
		Material();
		~Material();

		bool SaveToFile(const std::string& filepath) override;
		bool LoadFromFile(const std::string& filepath) override;

		// get & set
		dvTexture2D* GetAlbedo() { return m_pTexAlbedo; }
		void SetAlbedo(dvTexture2D* pAlbedo) { m_pTexAlbedo = pAlbedo; }

	private:
	private:
		dvTexture2D* m_pTexAlbedo = nullptr;
		float m_ColorAlbedo[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
}