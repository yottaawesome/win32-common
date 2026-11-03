export module Shared:Strings.Functions;
import std;
import Win32;
import :Error;

//
// General string utility functions.
export namespace Strings
{
	[[nodiscard]]
	auto TokeniseString(
		const std::string& stringToTokenise, 
		const std::string& delimiter
	) -> std::vector<std::string>
	{
		auto position = size_t{};
		// If we don't find it at all, add the whole string
		if (stringToTokenise.find(delimiter, position) == std::string::npos)
			return { stringToTokenise };

		auto results = std::vector<std::string>{};
		auto intermediateString = std::string{ stringToTokenise };
		while ((position = intermediateString.find(delimiter)) != std::string::npos)
		{
			// split and add to the results
			auto split = std::string{ intermediateString.substr(0, position) };
			results.push_back(split);

			// move up our position
			position += delimiter.length();
			intermediateString = intermediateString.substr(position);

			// On the last iteration, enter the remainder
			if (intermediateString.find(delimiter) == std::string::npos)
				results.push_back(intermediateString);
		}

		return results;
	}
}

//
// Conversion functions between std::string and std::wstring using Win32 API.
export namespace Strings
{
	auto Convert(const std::string& str) -> std::wstring
	{
		if (str.empty())
			return {};
		auto sizeNeeded = Win32::MultiByteToWideChar(
			Win32::CodePage::Utf8,
			0,
			str.data(),
			static_cast<int>(str.size()),
			nullptr,
			0
		);
		if (sizeNeeded <= 0)
			throw Error::Win32Error{ Win32::GetLastError(), "MultiByteToWideChar() failed [1]" };

		auto wstr = std::wstring(sizeNeeded, '\0');
		auto result = Win32::MultiByteToWideChar(
			Win32::CodePage::Utf8,
			0,
			str.data(),
			static_cast<int>(str.size()),
			wstr.data(),
			sizeNeeded
		);
		if (result <= 0)
			throw Error::Win32Error{ Win32::GetLastError(), "MultiByteToWideChar() failed [2]" };

		return wstr;
	}

	auto Convert(const std::wstring& wstr) -> std::string
	{
		if (wstr.empty())
			return {};
		auto sizeNeeded = Win32::WideCharToMultiByte(
			Win32::CodePage::Utf8,
			Win32::WideCharFlags::NoBestFitChars,
			wstr.data(),
			static_cast<int>(wstr.size()),
			nullptr,
			0,
			nullptr,
			nullptr
		);
		if (sizeNeeded <= 0)
			throw Error::Win32Error{ Win32::GetLastError(), "WideCharToMultiByte() failed [1]" };

		auto str = std::string(sizeNeeded, '\0');
		auto result = Win32::WideCharToMultiByte(
			Win32::CodePage::Utf8,
			Win32::WideCharFlags::NoBestFitChars,
			wstr.data(),
			static_cast<int>(wstr.size()),
			str.data(),
			sizeNeeded,
			nullptr,
			nullptr
		);
		if (result <= 0)
			throw Error::Win32Error{ Win32::GetLastError(), "WideCharToMultiByte() failed [2]" };

		return str;
	}
}

//
// std::formatter specializations for converting between narrow and wide strings
export namespace std
{
	// wchar_t -> char
	template<size_t N>
	struct formatter<wchar_t[N], char> : formatter<char, char>
	{
		template <class TContext>
		auto format(const wchar_t(str)[N], TContext&& ctx) const
		{
			return format_to(ctx.out(), "{}", Strings::Convert(str));
		}
	};

	template<>
	struct formatter<const wchar_t*, char> : formatter<char, char>
	{
		template <class TContext>
		auto format(const wchar_t* str, TContext&& ctx) const
		{
			return format_to(ctx.out(), "{}", Strings::Convert(str));
		}
	};

	template<>
	struct formatter<wstring, char> : formatter<char, char>
	{
		template <class TContext>
		auto format(const wstring& str, TContext&& ctx) const
		{
			return format_to(ctx.out(), "{}", Strings::Convert(str));
		}
	};

	template<>
	struct formatter<wstring_view, char> : formatter<char, char>
	{
		template <class TContext>
		auto format(wstring_view str, TContext&& ctx) const
		{
			return format_to(ctx.out(), "{}", Strings::Convert(str));
		}
	};

	// char -> wchar_t
	template<size_t N>
	struct formatter<char[N], wchar_t> : formatter<wchar_t, wchar_t>
	{
		template <class TContext>
		auto format(const char(str)[N], TContext&& ctx) const
		{
			return format_to(ctx.out(), L"{}", Strings::Convert(str));
		}
	};

	template<>
	struct formatter<const char*, wchar_t> : formatter<wchar_t, wchar_t>
	{
		template <class TContext>
		auto format(const char* str, TContext&& ctx) const
		{
			return format_to(ctx.out(), L"{}", Strings::Convert(str));
		}
	};

	template<>
	struct formatter<string, wchar_t> : formatter<wchar_t, wchar_t>
	{
		template <class TContext>
		auto format(const string& str, TContext&& ctx) const
		{
			return format_to(ctx.out(), L"{}", Strings::Convert(str));
		}
	};

	template<>
	struct formatter<string_view, wchar_t> : formatter<wchar_t, wchar_t>
	{
		template <class TContext>
		auto format(string_view str, TContext&& ctx) const
		{
			return format_to(ctx.out(), L"{}", Strings::Convert(str));
		}
	};
}