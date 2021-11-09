/*
	StringHelper 구현부
	
	제작: 서보윤( akahyulim@gmail.com )
*/

//= INCLUDES ====================
#include "StringHelper.h"
//===============================

namespace DiveEngine
{
	namespace StringHelper
	{
		std::string ToUppercase(const char * pLowerStr)
		{
			const int size = static_cast<int>(strlen(pLowerStr));

			std::string pUpperStr;
			pUpperStr.reserve(size);

			for (int i = 0; i != size; i++)
			{
				if (pLowerStr[i] >= 'a' && pLowerStr[i] <= 'z')
					pUpperStr.push_back(pLowerStr[i] - ('a' - 'A'));
				else
					pUpperStr.push_back(pLowerStr[i]);
			}

			return pUpperStr;
		}

		std::string ToLowercase(const char * pUpperStr)
		{
			const int size = static_cast<int>(strlen(pUpperStr));

			std::string pLowerStr;
			pLowerStr.reserve(size);

			for (int i = 0; i != size; i++)
			{
				if (pUpperStr[i] >= 'A' && pUpperStr[i] <= 'Z')
					pLowerStr.push_back(pUpperStr[i] + ('a' - 'A'));
				else
					pLowerStr.push_back(pUpperStr[i]);
			}

			return pLowerStr;
		}

		std::string ToUppercase(const std::string & lowerStr)
		{
			std::locale loc;
			std::string upperStr;
			for (const auto& character : lowerStr)
			{
				upperStr += toupper(character, loc);
			}

			return upperStr;
		}

		std::string ToLowercase(const std::string & upperStr)
		{
			std::locale loc;
			std::string lowerStr;
			for (const auto& character : upperStr)
			{
				lowerStr += tolower(character, loc);
			}

			return lowerStr;
		}

		// ---------------------------------------------------------------------------------------------------------
		// 주의: 최초 토큰에서 문자열 끝까지 리턴합니다.
		// ex) "Our love is great" "love" => "love is great"
		// ---------------------------------------------------------------------------------------------------------
		std::string GetStringAfterToken(const char * pStr, const char * pToken)
		{
			const int tokenSize = static_cast<int>(strlen(pToken));
			char cmpStr[256] = { 0, };
			std::string afterStr;

			while (*pStr != '\0')
			{
				if (*pStr == *pToken)
				{
					memset(cmpStr, 0, sizeof(cmpStr));
					memcpy(cmpStr, pStr, tokenSize);

					if (0 == strcmp(cmpStr, pToken))
						afterStr = pStr;
				}

				pStr++;
			}

			return afterStr;
		}

		// ---------------------------------------------------------------------------------------------------------
		// 주의: FirstToken, SecondToken 둘 다 처음 검색된 동일 토큰을 기준으로 작동합니다.
		// ex) "Next Song: Plastic Love. Next Song: Plastic Love. " "Song: " " Next" => "Plastic Love."
		// ---------------------------------------------------------------------------------------------------------
		std::string GetStringBetweenTokens(const char * pStr, const char * pFirstToken, const char * pSecondToken)
		{
			const int firstTokenSize = static_cast<int>(strlen(pFirstToken));
			const int secondTokenSize = static_cast<int>(strlen(pSecondToken));

			char cmpStr[256] = { 0, };

			bool isFoundFirstToken = false;

			std::string betweenStr;

			while (*pStr != '\0')
			{
				if (*pStr == *pFirstToken)
				{
					if (!isFoundFirstToken)
					{
						memset(cmpStr, 0, sizeof(cmpStr));
						memcpy(cmpStr, pStr, firstTokenSize);

						if (0 == strcmp(cmpStr, pFirstToken))
						{
							isFoundFirstToken = true;
							pStr += firstTokenSize;
						}
					}
				}

				// 첫 번째 토큰 후 바로 위치할 수 있다.
				if (*pStr == *pSecondToken)
				{
					if (isFoundFirstToken)
					{
						memset(cmpStr, 0, sizeof(cmpStr));
						memcpy(cmpStr, pStr, secondTokenSize);

						if (0 == strcmp(cmpStr, pSecondToken))
						{
							if (betweenStr.empty())
							{
								betweenStr.clear();
							}

							break;
						}
					}
				}

				if (isFoundFirstToken)
					betweenStr += *pStr;

				pStr++;
			}

			return betweenStr;
		}

		std::string ReplaceToken(const char * pStr, const char * pToken, const char * pNewToken)
		{
			const int tokenSize = static_cast<int>(strlen(pToken));
			char cmpStr[256] = { 0, };
			std::string fixedStr;

			while (*pStr != '\0')
			{
				if (*pStr == *pToken)
				{
					memset(cmpStr, 0, sizeof(cmpStr));
					memcpy(cmpStr, pStr, tokenSize);

					if (0 == strcmp(cmpStr, pToken))
					{
						fixedStr += pNewToken;
						pStr += tokenSize - 1;
					}
					else
						fixedStr += *pStr;
				}
				else
					fixedStr += *pStr;

				pStr++;
			}

			return fixedStr;
		}

		std::wstring MbcsToWbcs(const std::string str, unsigned int mbcsEncoding)
		{
			const int length = MultiByteToWideChar(mbcsEncoding, 0, str.c_str(), static_cast<int>(str.length()), 0, 0);

			std::vector<wchar_t> utf16Buffer;
			utf16Buffer.resize(length + 1);

			MultiByteToWideChar(mbcsEncoding, 0, str.c_str(), (int)str.length(), &utf16Buffer[0], (int)utf16Buffer.size());

			return &utf16Buffer[0];
		}

		std::string WbcsToMbcs(const std::wstring str, unsigned int mbcsEncoding)
		{
			const int length = WideCharToMultiByte(mbcsEncoding, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0, NULL, NULL);

			std::vector<char> outBuffer;
			outBuffer.resize(length + 1);

			WideCharToMultiByte(mbcsEncoding, 0, str.c_str(), (int)str.length(), &outBuffer[0], (int)outBuffer.size(), 0, 0);

			return &outBuffer[0];
		}

		std::string ConvertMbcs(const std::string str, unsigned int fromEncoding, unsigned int toEncoding)
		{
			std::wstring utf16Str = MbcsToWbcs(str, fromEncoding);

			return WbcsToMbcs(utf16Str, toEncoding);
		}

		std::wstring Utf8ToUtf16(const std::string str)
		{
			return MbcsToWbcs(str, CP_UTF8);
		}

		std::string Utf16ToUtf8(const std::wstring str)
		{
			return WbcsToMbcs(str, CP_UTF8);
		}
	}
}