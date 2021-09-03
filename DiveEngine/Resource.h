#pragma once
#include "Object.h"
#include <string>

namespace dive
{
	class Resource : public Object
	{
	public:
		Resource(size_t typeHash) : Object(typeHash) {}
		virtual ~Resource() = default;

		// ���� ���� �Լ��̸� �ȵǳ�?
		virtual bool LoadFromFile(const std::string& filepath) { return true; }
		virtual bool SaveToFile(const std::string& filepath) { return true; }


		// get resource type


	protected:

		// ���ĸ�ź�� ���丮���� ���� �����Ѵ�.
		// �׸��� ���� Object name�� ����� Resource Name�̶��� ������...
	};
}