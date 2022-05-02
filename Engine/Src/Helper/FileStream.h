#pragma once

namespace Dive
{
	enum eFileStreamMode : uint32_t
	{
		Read	= 1 << 0,
		Write	= 1 << 1,
		Append	= 1 << 2
	};

	class FileStream
	{
	public:
		FileStream(const std::string& filepath, uint32_t flags);
		~FileStream();

		bool IsOpen() const { return m_bOpen; }
		void Close();

		// write
        template <class T, class = typename std::enable_if <
            std::is_same<T, bool>::value ||
            std::is_same<T, unsigned char>::value ||
            std::is_same<T, int>::value ||
            std::is_same<T, long>::value ||
            std::is_same<T, long long>::value ||
            std::is_same<T, uint8_t>::value ||
            std::is_same<T, uint16_t>::value ||
            std::is_same<T, uint32_t>::value ||
            std::is_same<T, uint64_t>::value ||
            std::is_same<T, unsigned long>::value ||
            std::is_same<T, unsigned long long>::value ||
            std::is_same<T, float>::value ||
            std::is_same<T, double>::value ||
            std::is_same<T, long double>::value ||
            std::is_same<T, std::byte>::value ||
            std::is_same<T, DirectX::XMFLOAT2>::value ||
            std::is_same<T, DirectX::XMFLOAT3>::value ||
            std::is_same<T, DirectX::XMFLOAT4>::value
        > ::type >
        void Write(T value)
        {
            m_fOut.write(reinterpret_cast<char*>(&value), sizeof(value));
        }

        void Write(const std::string& value);
        void Write(const std::vector<std::string>& value);
        //void Write(const std::vector<RHI_Vertex_PosTexNorTan>& value);
        void Write(const std::vector<uint32_t>& value);
        void Write(const std::vector<unsigned char>& value);
        void Write(const std::vector<std::byte>& value);
        void Skip(uint64_t n);

		// read
        template <class T, class = typename std::enable_if<
            std::is_same<T, bool>::value ||
            std::is_same<T, unsigned char>::value ||
            std::is_same<T, int>::value ||
            std::is_same<T, long>::value ||
            std::is_same<T, long long>::value ||
            std::is_same<T, uint8_t>::value ||
            std::is_same<T, uint16_t>::value ||
            std::is_same<T, uint32_t>::value ||
            std::is_same<T, uint64_t>::value ||
            std::is_same<T, unsigned long>::value ||
            std::is_same<T, unsigned long long>::value ||
            std::is_same<T, float>::value ||
            std::is_same<T, double>::value ||
            std::is_same<T, long double>::value ||
            std::is_same<T, std::byte>::value ||
            std::is_same<T, DirectX::XMFLOAT2>::value ||
            std::is_same<T, DirectX::XMFLOAT3>::value ||
            std::is_same<T, DirectX::XMFLOAT4>::value
        > ::type >
        void Read(T* value)
        {
            m_fIn.read(reinterpret_cast<char*>(value), sizeof(T));
        }

        void Read(std::string* pValue);
        void Read(std::vector<std::string>* pValue);
        //void Read(std::vector<RHI_Vertex_PosTexNorTan>* vec);
        void Read(std::vector<uint32_t>* pValue);
        void Read(std::vector<unsigned char>* pValue);
        void Read(std::vector<std::byte>* pValue);

        template <class T, class = typename std::enable_if
            <
            std::is_same<T, bool>::value ||
            std::is_same<T, unsigned char>::value ||
            std::is_same<T, int>::value ||
            std::is_same<T, long>::value ||
            std::is_same<T, long long>::value ||
            std::is_same<T, uint8_t>::value ||
            std::is_same<T, uint16_t>::value ||
            std::is_same<T, uint32_t>::value ||
            std::is_same<T, uint64_t>::value ||
            std::is_same<T, unsigned long>::value ||
            std::is_same<T, unsigned long long>::value ||
            std::is_same<T, float>::value ||
            std::is_same<T, double>::value ||
            std::is_same<T, long double>::value ||
            std::is_same<T, std::byte>::value ||
            std::is_same<T, std::string>::value
        >::type>
        T ReadAs()
        {
            T value;
            Read(&value);
            return value;
        }

	private:
		std::ofstream m_fOut;
		std::ifstream m_fIn;
		uint32_t m_Flags;
		bool m_bOpen;
	};
}