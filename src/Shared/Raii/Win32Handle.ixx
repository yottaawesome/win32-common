export module Shared:Raii.Win32Handle;
import std;
import Win32;
import :Error;

export namespace Raii
{
	class Win32Handle
	{
	public:
		~Win32Handle()
		{
			Close();
		}
		Win32Handle() noexcept = default;
		Win32Handle(Win32::HANDLE handle) noexcept
			: m_handle(handle)
		{}

		Win32Handle(const Win32Handle&) = delete;
		Win32Handle& operator=(const Win32Handle&) = delete;

		Win32Handle(Win32Handle&& other) noexcept
			: m_handle(std::exchange(other.m_handle, nullptr))
		{}
		auto operator=(Win32Handle&& other) noexcept -> Win32Handle&
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
			if (IsValid())
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

		constexpr void reset(Win32::HANDLE handle = nullptr) noexcept
		{
			if (m_handle == handle)
				return;
			CloseHandle(m_handle);
			m_handle = handle;
		}

		void SetInheritance(bool inherit)
		{
			SetInheritance(m_handle, inherit);
		}

		static void SetInheritance(Win32::HANDLE handle, bool inherit)
		{
			if (handle == nullptr or handle == Win32::InvalidHandleValue)
				throw Error::RuntimeError{ "Invalid handle" };
			auto succeeded = Win32::SetHandleInformation(handle, Win32::HandleFlags::Inherit, inherit ? Win32::HandleFlags::Inherit : 0);
			if (not succeeded)
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to set handle inheritance" };
		}

		[[nodiscard]]
		static auto DuplicateHandle(Win32::HANDLE handle) -> Win32Handle
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
			return Win32Handle{ duplicatedHandle };
		}

	private:
		Win32::HANDLE m_handle = nullptr;
	};
}
