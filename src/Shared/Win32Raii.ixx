export module Shared:Win32Raii;
import std;
import Win32;

export namespace Win32
{
	template<auto VDeleter>
	struct Deleter
	{
		constexpr void operator()(auto&& ptr) const noexcept
		{
			VDeleter(ptr);
		}
	};
	template<typename T, auto VDeleter>
	using DirectUniquePtr = std::unique_ptr<T, Deleter<VDeleter>>;
	template<typename T, auto VDeleter>
	using IndirectUniquePtr = std::unique_ptr<std::remove_pointer_t<T>, Deleter<VDeleter>>;

	using HandleUniquePtr = IndirectUniquePtr<std::remove_pointer_t<Win32::HANDLE>, Win32::CloseHandle>;
	using HwndUniquePtr = IndirectUniquePtr<std::remove_pointer_t<Win32::HWND>, Win32::DestroyWindow>;
}
