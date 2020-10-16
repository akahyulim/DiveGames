#include "DivePch.h"
#include "StringUtils.h"

namespace Dive
{
	std::string ToUppercase(const char * lowerStr)
	{
		int size = static_cast<int>(strlen(lowerStr));

		std::string upperStr;
		upperStr.reserve(size);

		for (int i = 0; i != size; i++)
		{
			if (lowerStr[i] >= 'a' && lowerStr[i] <= 'z')
				upperStr.push_back(lowerStr[i] - ('a' - 'A'));
			else
				upperStr.push_back(lowerStr[i]);
		}

		return upperStr;
	}

	std::string ToLowercase(const char * upperStr)
	{
		int size = static_cast<int>(strlen(upperStr));

		std::string lowerStr;
		lowerStr.reserve(size);

		for (int i = 0; i != size; i++)
		{
			if (upperStr[i] >= 'A' && upperStr[i] <= 'Z')
				lowerStr.push_back(upperStr[i] + ('a' - 'A'));
			else
				lowerStr.push_back(upperStr[i]);
		}

		return lowerStr;
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
	std::string GetStringAfterToken(const char * str, const char * token)
	{
		int tokenSize = static_cast<int>(strlen(token));
		char cmpStr[256];
		std::string afterStr;

		while (*str != '\0')
		{
			if (*str == *token)
			{
				memset(cmpStr, 0, sizeof(cmpStr));
				memcpy(cmpStr, str, tokenSize);

				if (0 == strcmp(cmpStr, token))
					afterStr = str;
			}

			str++;
		}

		return afterStr;
	}

	// ---------------------------------------------------------------------------------------------------------
	// 주의: FirstToken, SecondToken 둘 다 처음 검색된 동일 토큰을 기준으로 작동합니다.
	// ex) "Next Song: Plastic Love." "Next" " Love" => " Song: Plastic ."
	// ---------------------------------------------------------------------------------------------------------
	std::string GetStringBetweenTokens(const char * str, const char * firstToken, const char * secondToken)
	{
		int firstTokenSize = static_cast<int>(strlen(firstToken));
		int secondTokenSize = static_cast<int>(strlen(secondToken));

		char cmpStr[256];

		bool isFoundFirstToken = false;

		std::string betweenStr;

		while (*str != '\0')
		{
			if (*str == *firstToken)
			{
				if (!isFoundFirstToken)
				{
					memset(cmpStr, 0, sizeof(cmpStr));
					memcpy(cmpStr, str, firstTokenSize);

					if (0 == strcmp(cmpStr, firstToken))
					{
						isFoundFirstToken = true;
						str += firstTokenSize;
					}
				}
			}

			// 첫 번째 토큰 후 바로 위치할 수 있다.
			if (*str == *secondToken)
			{
				if (isFoundFirstToken)
				{
					memset(cmpStr, 0, sizeof(cmpStr));
					memcpy(cmpStr, str, secondTokenSize);

					if (0 == strcmp(cmpStr, secondToken))
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
				betweenStr += *str;

			str++;
		}

		return betweenStr;
	}

	std::string ReplaceToken(const char * str, const char * token, const char * newToken)
	{
		int tokenSize = static_cast<int>(strlen(token));
		char cmpStr[256];
		std::string fixedStr;

		while (*str != '\0')
		{
			if (*str == *token)
			{
				memset(cmpStr, 0, sizeof(cmpStr));
				memcpy(cmpStr, str, tokenSize);

				if (0 == strcmp(cmpStr, token))
				{
					fixedStr += newToken;
					str += tokenSize - 1;
				}
				else
					fixedStr += *str;
			}
			else
				fixedStr += *str;

			str++;
		}

		return fixedStr;
	}

	std::wstring MbcsToWbcs(const std::string str, unsigned int mbcsEncoding)
	{
		int length = MultiByteToWideChar(mbcsEncoding, 0, str.c_str(), static_cast<int>(str.length()), 0, 0);

		std::vector<wchar_t> utf16Buffer;
		utf16Buffer.resize(length + 1);

		MultiByteToWideChar(mbcsEncoding, 0, str.c_str(), (int)str.length(), &utf16Buffer[0], (int)utf16Buffer.size());

		return &utf16Buffer[0];
	}

	std::string WbcsToMbcs(const std::wstring str, unsigned int mbcsEncoding)
	{
		int length = WideCharToMultiByte(mbcsEncoding, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0, NULL, NULL);

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