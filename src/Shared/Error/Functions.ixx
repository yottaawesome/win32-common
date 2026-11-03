export module Shared:Error.Functions;
import std;
import Win32;

//
//
// Error utility functions for Win32 error codes.
export namespace Error
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