#pragma once
#include "Object.h"
#include <string>

namespace dive
{
	enum class eResourceType
	{
		Unknown,
		Texture2D,
	};

	// eResourceType�� �����°� �´� �� ����.
	// �׷��� Object�� TypeHash�� �� �ȸ´� �� ����.
	class Resource : public Object
	{
	public:
		Resource(size_t typeHash) : Object(typeHash) {}
		virtual ~Resource() = default;

		// ���� ���� �Լ��̸� �ȵǳ�?
		virtual bool LoadFromFile(const std::string& filepath) { return true; }
		//virtual bool SaveToFile(const std::string& filepath) { return true; }


		eResourceType GetResourceType() const { return m_Type; }


	protected:
		eResourceType m_Type = eResourceType::Unknown;

		// ���ĸ�ź�� ���丮���� ���� �����Ѵ�.
		// �׸��� ���� Object name�� ����� Resource Name�̶��� ������...
	};
}