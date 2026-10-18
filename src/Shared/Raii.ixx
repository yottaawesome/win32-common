export module Shared:Raii;
import std;
import Win32;
import :Error;

export namespace Raii
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

	class Handle
	{
	public:
		~Handle()
		{
			Close();
		}
		Handle() noexcept = default;
		Handle(Win32::HANDLE handle) noexcept
			: m_handle(handle)
		{}

		Handle(const Handle&) = delete;
		Handle& operator=(const Handle&) = delete;

		Handle(Handle&& other) noexcept
			: m_handle(std::exchange(other.m_handle, nullptr))
		{}
		auto operator=(Handle&& other) noexcept -> Handle&
		{
			if (this != &other)
			{
				Close();
				m_handle = std::exchange(other.m_handle, nullptr);
			}
			return *this;
		}

		void Close() noexcept
		{
			if (m_handle)
			{
				Win32::CloseHandle(m_handle);
				m_handle = nullptr;
			}
		}

		[[nodiscard]]
		constexpr auto IsValid() const noexcept -> bool
		{
			return m_handle != nullptr and m_handle != Win32::InvalidHandleValue;
		}

		[[nodiscard]]
		constexpr operator bool() const noexcept
		{
			return IsValid();
		}

		[[nodiscard]] 
		constexpr auto get() const noexcept -> Win32::HANDLE
		{
			return m_handle;
		}

		constexpr auto reset(Win32::HANDLE handle = nullptr) noexcept -> Win32::HANDLE
		{
			return std::exchange(m_handle, handle);
		}

		void SetInheritance(bool inherit)
		{
			auto succeeded = Win32::SetHandleInformation(m_handle, Win32::HandleFlags::Inherit, inherit ? Win32::HandleFlags::Inherit : 0);
			if (not succeeded)
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to set handle inheritance" };
		}

		[[nodiscard]]
		static auto DuplicateHandle(Win32::HANDLE handle) -> Handle
		{
			auto duplicatedHandle = Win32::HANDLE{};
			auto succeeded = Win32::DuplicateHandle(
				Win32::GetCurrentProcess(),
				handle,
				Win32::GetCurrentProcess(),
				&duplicatedHandle,
				0,
				false,
				Win32::DuplicateHandleFlags::SameAccess
			);
			if (not succeeded)
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to duplicate handle" };
			return Handle{ duplicatedHandle };
		}

	private:
		Win32::HANDLE m_handle = nullptr;
	};
}
