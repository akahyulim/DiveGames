#pragma once
#include "GraphicsInclude.h"
#include <fstream>
#include <string>
#include <vector>

namespace Dive
{
	enum class eFileStreamMode
	{
		Read,
		Write,
		Append,
	};

	class FileStream
	{
	public:
		FileStream(const std::string& filepath, eFileStreamMode mode);
		~FileStream();

		bool IsOpen() const { return m_bOpened; }
		void Close();

		// 읽기
		template<class T, class = typename std::enable_if <
			std::is_same<T, bool>::value ||
			std::is_same<T, unsigned char>::value ||
			std::is_same<T, int>::value ||
			std::is_same<T, unsigned int>::value ||
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, std::byte>::value ||
			std::is_same<T, DirectX::XMFLOAT2>::value ||
			std::is_same<T, DirectX::XMFLOAT3>::value ||
			std::is_same<T, DirectX::XMFLOAT4>::value
			>::type>
		void Read(T* value)
		{
			m_in.read(reinterpret_cast<char*>(value), sizeof(T));
		}

		template<class T, class = typename std::enable_if <
			std::is_same<T, bool>::value ||
			std::is_same<T, unsigned char>::value ||
			std::is_same<T, int>::value ||
			std::is_same<T, unsigned int>::value ||
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, std::byte>::value ||
			std::is_same<T, DirectX::XMFLOAT2>::value ||
			std::is_same<T, DirectX::XMFLOAT3>::value ||
			std::is_same<T, DirectX::XMFLOAT4>::value
		>::type>
		T ReadAs()
		{
			T value;
			Read(&value);
			return value;
		}

		void Read(std::string* value);
		void Read(std::vector<std::string>* vec);
		void Read(std::vector<unsigned char>* vec);
		void Read(std::vector<std::byte>* vec);
		void Read(std::vector<unsigned int>* vec);
	//	void Read(std::vector<Vertex_PosTexNorTan>* vec);


		// 쓰기
		template<class T, class = typename std::enable_if <
			std::is_same<T, bool>::value ||
			std::is_same<T, unsigned char>::value ||
			std::is_same<T, int>::value ||
			std::is_same<T, unsigned int>::value ||
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, std::byte>::value ||
			std::is_same<T, DirectX::XMFLOAT2>::value ||
			std::is_same<T, DirectX::XMFLOAT3>::value ||
			std::is_same<T, DirectX::XMFLOAT4>::value
		>::type>
		void Write(T value)
		{
			m_out.write(reinterpret_cast<char*>(&value), sizeof(T));
		}

		void Write(const std::string& value);
		void Write(const std::vector<std::string>& vec);
		void Write(const std::vector<unsigned char>& vec);
		void Write(const std::vector<std::byte>& vec);
		void Write(const std::vector<unsigned int>& vec);
	//	void Write(const std::vector<Vertex_PosTexNorTan>& vec);

	private:
		std::ofstream m_out;
		std::ifstream m_in;
		eFileStreamMode m_mode;
		bool m_bOpened;
	};
}