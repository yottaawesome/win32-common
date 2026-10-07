export module Shared:Win32Error;
import std;
import Win32;

//
//
// Error utility functions for Win32 error codes.
export namespace Win32::Error
{
	auto Win32CodeToString(Win32::DWORD code) -> std::string
	{
		auto buffer = static_cast<void*>(nullptr);
		auto chars =
			Win32::FormatMessageA(
				Win32::FormatMessageFlags::FromSystem | Win32::FormatMessageFlags::AllocateBuffer,
				nullptr,
				code,
				0,
				reinterpret_cast<Win32::LPSTR>(&buffer),
				0,
				nullptr
			);
		if (chars == 0 or not buffer)
			return std::format("Unknown error code: {}", code);

		auto message = std::string{ static_cast<char*>(buffer), chars };
		Win32::LocalFree(buffer);

		while (not message.empty() and (message.back() == '\r' or message.back() == '\n'))
			message.pop_back();
		return message;
	}
}

//
//
// Exceptions for Win32 and runtime errors.
export namespace Win32::Error
{
	//
	// Basic runtime error. Includes source location information in the error message.
	class RuntimeError : public std::runtime_error
	{
	public:
		struct PlainMessage
		{
			std::string Message;
		};
		explicit RuntimeError(PlainMessage message)
			: std::runtime_error(message.Message)
		{}
		explicit RuntimeError(std::string_view message, const std::source_location& loc = std::source_location::current())
			: std::runtime_error(Format(message, loc))
		{}
	private:
		static auto Format(std::string_view message, const std::source_location& loc) -> std::string
		{
			return std::format("{} at {}:{}:{}", message, loc.file_name(), loc.line(), loc.column());
		}
	};

	//
	// Win32 error. Includes the Win32 error code and source location information in the error message.
	class Win32Error : public RuntimeError
	{
	public:
		explicit Win32Error(Win32::DWORD code, const std::source_location& loc = std::source_location::current())
			: RuntimeError(PlainMessage{ Format(code, loc) })
			, code(code)
		{}

		explicit Win32Error(Win32::DWORD code, std::string_view message, const std::source_location& loc = std::source_location::current())
			: RuntimeError(PlainMessage{ Format(code, message, loc) })
			, code(code)
		{}

		constexpr auto Code() const noexcept -> Win32::DWORD
		{
			return code;
		}

	private:
		static auto Format(Win32::DWORD code, const std::source_location& loc) -> std::string
		{
			return std::format("Win32 error {} ({}) at {}:{}:{}", Win32CodeToString(code), code, loc.file_name(), loc.line(), loc.column());
		}

		static auto Format(Win32::DWORD code, std::string_view message, const std::source_location& loc) -> std::string
		{
			return std::format("{} - Win32 error {} ({}) at {}:{}:{}", message, Win32CodeToString(code), code, loc.file_name(), loc.line(), loc.column());
		}

	private:
		Win32::DWORD code = 0;
	};
}