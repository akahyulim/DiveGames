#pragma once
#include "../Core/Object.h"
#include <string>

namespace Dive
{
	enum class eResourceType
	{
		Unknown,
		Texture2D,
		Mesh,
		Static_Mesh,
		Skinned_Mesh,
		Material
	};

	// eResourceType�� �����°� �´� �� ����.
	// �׷��� Object�� TypeHash�� �� �ȸ´� �� ����.
	class Resource : public Object
	{
	public:
		Resource();
		virtual ~Resource() = default;

		// �̷��� �ϸ� ���� ����ȭ�ϰڴٴ� �Ŵ�.
		virtual bool SaveToFile(const std::string& filepath) { return true; }
		virtual bool LoadFromFile(const std::string& filepath) { return true; }
		

		eResourceType GetType() const { return m_Type; }

		template<class T>
		static constexpr eResourceType TypeToEnum();

	protected:
		eResourceType m_Type = eResourceType::Unknown;

		// ���ĸ�ź�� ���丮���� ���� �����Ѵ�.
		// �׸��� ���� Object name�� ����� Resource Name�̶��� ������...
	};
}