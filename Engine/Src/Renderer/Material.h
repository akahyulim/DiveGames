#pragma once
#include "../Resource/Resource.h"
#include <string>

namespace Dive
{
	class dvTexture2D;

	/*
	*	���ĸ�ź�� ���
	*	Resource�� ����Ѵ�.
	*	���� ������(����, Texture ���, ��Ÿ)�� XML ���Ϸ� Save, Load�Ѵ�.
	*	Load�� ��� Texture�� ResourceCache�κ��� �̸��� �����Ͽ� �����´�.
	*	���� Load�Ǿ� ���� �ʴٸ� ���� Load�Ѵ�.
	*	���� ������ ���� Mesh���� Model���� �𸣰ڴ�.
	*	Albedo ������ Mesh ������ �������� ��쵵 �ֱ� �����̴�.
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