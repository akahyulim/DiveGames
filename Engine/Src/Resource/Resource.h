#pragma once
#include "Base/Object.h"

namespace Dive
{
	static const char* EXTENSION_MODEL		= "model";
	static const char* EXTENSION_TEXTURE	= "texture";
	static const char* EXTENSION_MATERIAL	= "mtrl";

	enum class eResourceType
	{
		Texture,
		Texture2D,
		Model,
		Material,
		Count
	};

#define RESOURCE_CLASS_TYPE(type)	static eResourceType GetStaticType() { return eResourceType::type;} \
									virtual eResourceType GetType() const override { return GetStaticType(); } \
									virtual const char* GetTypeStr() const override { return #type;} 
 
	class Resource : public Object
	{
	public:
		Resource(const std::string& name = "", unsigned long long id = 0);
		virtual ~Resource() = default;

		virtual bool SaveToFile(const std::string& filepath) { return true; }
		virtual bool LoadFromFile(const std::string& filepath)	{ return true; }

		virtual eResourceType GetType()		const = 0;
		virtual const char* GetTypeStr()	const = 0;

		const std::string GetExportedFilepath() const { return m_ExportedFilepath; }
		const std::string GetEngineFilepath() const { return m_EngineFilepath; }

		// ���� Model�� ��� Save, Load���� �ϵ� �ڵ� �س��Ҵ�.
		void SetFilepath(const std::string& filepath)
		{
			// ���� ������ ���
			// ���� ���� ��θ� ������ �� �����Ѵ�.
			// �̶� ���� ���亰�� ���� ������ ����� �Լ��� �ʿ��ϴ�.

			// ���� ������ ���
			// ���� ���� ��δ� �����Ƿ� �׳� clear�Ѵ�.

			// ���ҽ��� �̸��� �� ������ �ڵ����� �����ȴ�.
			
			// Ÿ�Ժ� ���� ��θ� �����Ѵ�.
			// ����ο��� �̾Ƴ��� �� ����.
		}
		
	protected:
		std::string m_ExportedFilepath;
		std::string m_EngineFilepath;
	};
}