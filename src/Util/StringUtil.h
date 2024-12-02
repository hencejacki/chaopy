#pragma once
#include <string>
#include <locale>
#include <codecvt>

namespace StringUtil
{
	static inline std::wstring ToWString(const std::string& narrowStr)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(narrowStr);
	}

	static inline std::string FromWString(const std::wstring& wideStr)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(wideStr);
	}
}