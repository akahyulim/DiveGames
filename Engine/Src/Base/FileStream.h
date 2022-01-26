#pragma once
#include "Renderer/Graphics/GraphicsInclude.h"
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

		bool IsOpen() const { return m_bFileOpened; }
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
			m_InputStream.read(reinterpret_cast<char*>(value), sizeof(T));
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

		void Read(std::string* pValue);
		void Read(std::vector<std::string>* pVec);
		void Read(std::vector<unsigned char>* pVec);
		void Read(std::vector<std::byte>* pVec);
		void Read(std::vector<unsigned int>* pVec);
	//	void Read(std::vector<Vertex_PosTexNorTan>* pVec);


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
			m_OutputStream.write(reinterpret_cast<char*>(&value), sizeof(T));
		}

		void Write(const std::string& value);
		void Write(const std::vector<std::string>& vec);
		void Write(const std::vector<unsigned char>& vec);
		void Write(const std::vector<std::byte>& vec);
		void Write(const std::vector<unsigned int>& vec);
	//	void Write(const std::vector<Vertex_PosTexNorTan>& vec);

	private:
		std::ofstream m_OutputStream;
		std::ifstream m_InputStream;
		eFileStreamMode m_StreamMode;
		bool m_bFileOpened;
	};
}