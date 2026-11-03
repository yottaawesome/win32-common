export module Shared:Error.ErrorTypes;
import std;
import Win32;
import :Error.Functions;

//
//
// Exceptions for Win32 and runtime errors.
export namespace Error
{
	struct PlainMessage
	{
		std::string Message;
	};

	//
	// Basic runtime error. Includes source location information in the error message.
	class RuntimeError : public std::runtime_error
	{
	public:
		explicit RuntimeError(PlainMessage message)
			: std::runtime_error(message.Message)
		{}
		explicit RuntimeError(
			std::string_view message,
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& stack = std::stacktrace::current()
		)
			: std::runtime_error(Format(message, loc, stack))
		{}
	private:
		static auto Format(std::string_view message, const std::source_location& loc, const std::stacktrace& stack) -> std::string
		{
			return std::format("{} at {}:{}:{}\n{}", message, loc.file_name(), loc.line(), loc.column(), stack);
		}
	};

	//
	// Win32 error. Includes the Win32 error code and source location information in the error message.
	class Win32Error : public RuntimeError
	{
	public:
		explicit Win32Error(Win32::DWORD code, PlainMessage message)
			: RuntimeError{ message }
			, code{ code }
		{}

		explicit Win32Error(
			Win32::DWORD code,
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& stack = std::stacktrace::current()
		) : RuntimeError{ PlainMessage{ Format(code, loc, stack) } }
			, code{ code }
		{}

		explicit Win32Error(
			Win32::DWORD code,
			std::string_view message,
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& stack = std::stacktrace::current()
		) : RuntimeError{ PlainMessage{ Format(code, message, loc, stack) } }
			, code{ code }
		{}

		constexpr auto Code() const noexcept -> Win32::DWORD
		{
			return code;
		}

	private:
		static auto Format(
			Win32::DWORD code,
			const std::source_location& loc,
			const std::stacktrace& stack
		) -> std::string
		{
			return std::format("Win32 error {} ({}) at {}:{}:{}\n{}", Win32CodeToString(code), code, loc.file_name(), loc.line(), loc.column(), stack);
		}

		static auto Format(
			Win32::DWORD code,
			std::string_view message,
			const std::source_location& loc,
			const std::stacktrace& stack
		) -> std::string
		{
			return std::format("{} - Win32 error {} ({}) at {}:{}:{}\n{}", message, Win32CodeToString(code), code, loc.file_name(), loc.line(), loc.column(), stack);
		}

	private:
		Win32::DWORD code = 0;
	};
}
