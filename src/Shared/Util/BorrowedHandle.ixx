export module Shared:Util.BorrowedHandle;
import std;
import Win32;

export namespace Util
{
	// A read-only handle wrapper that does not own the handle and does not close it on destruction.
	// Do not close or modify the handle. It is intended for use in situations where 
	// a handle is exposed for native functions, but  the lifetime is managed elsewhere, 
	// such as in the owning class.
	class BorrowedHandle final
	{
	public:
		constexpr BorrowedHandle(Win32::HANDLE handle) noexcept
			: m_handle{ handle }
		{}
		explicit constexpr operator Win32::HANDLE() const noexcept
		{
			return m_handle;
		}
		constexpr auto HasValue() const noexcept -> bool
		{
			return m_handle != nullptr;
		}
		constexpr auto Get() const noexcept -> Win32::HANDLE
		{
			return m_handle;
		}
		constexpr operator bool() const noexcept
		{
			return HasValue();
		}
		constexpr auto operator==(const BorrowedHandle& other) const noexcept -> bool
		{
			return m_handle == other.m_handle;
		}
	private:
		Win32::HANDLE m_handle = nullptr;
	};
}

namespace
{
	using namespace Util;
	static_assert(
		not BorrowedHandle{ nullptr }.HasValue(),
		"expected handle to not have value");
	static_assert(
		[] -> bool
		{
			auto x = 0;
			auto ptr = static_cast<void*>(&x);
			auto handle = BorrowedHandle{ ptr };
			if (not handle.HasValue())
				throw "expected handle to have value";
			if (handle.Get() != ptr)
				throw "expected handle to return the same pointer";
			if (static_cast<Win32::HANDLE>(handle) != ptr)
				throw "expected handle to convert to the same pointer";
			return true;
		}());
}
