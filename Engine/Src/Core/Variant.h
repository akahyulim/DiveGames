#pragma once
#include <variant>
#include <vector>
#include <map>
#include <string>

namespace Dive
{
	struct DvWindowEvent
	{
		HWND hWnd;
		UINT msg;
		WPARAM wParam;
		LPARAM lParam;
	};

#define _VARIANT_TYPES	\
	char,	\
	unsigned char,	\
	int,	\
	unsigned int,	\
	float,	\
	long,	\
	bool,	\
	void*,	\
	std::string

#define VARIANT_TYPES std::variant<_VARIANT_TYPES>
typedef std::variant<_VARIANT_TYPES, VARIANT_TYPES> StdVariant;

	class Variant;
	class StringHash;

	using VariantVector = std::vector<Variant>;
	using VariantMap = std::map<StringHash, Variant>;
	
	class Variant
	{
	public:
		Variant() = default;
		Variant(const Variant& rhs) { m_Variant = rhs.GetVariantRaw(); }
		template<class T, class = std::enable_if<!std::is_same<T, Variant>::value>>
		Variant(T value) { m_Variant = value; }
		~Variant() = default;

		Variant& operator=(const Variant& rhs);
		template<class T, class = std::enable_if<!std::is_same<T, Variant>::value>>
		Variant& operator=(T rhs) { return m_Variant = rhs; }
	
		const StdVariant& GetVariantRaw() const { return m_Variant; }

		template<class T>
		const T& Get() const { return std::get<T>(m_Variant); }

	private:
		StdVariant m_Variant;
	};
}