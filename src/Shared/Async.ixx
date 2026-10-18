export module Shared:Async;
import std;
import Win32;
import :Concepts;
import :Raii;

export namespace Async
{
	struct EventCreateParams
	{
		bool IsSignaled = false;
		bool IsInheritable = false;
		std::optional<std::wstring> Name = std::nullopt;
	};

	struct EventOpenParams
	{
		bool IsInheritable = false;
		std::wstring Name;
		Win32::DWORD DesiredAccess = Win32::Synchronize;
	};

	/// Encapsulates a Win32 Event synchronization object.
	template<bool VIsManualReset = false>
	class Event final
	{
	public:
		Event(const Event& other) = delete;
		auto operator=(const Event& other) -> Event & = delete;
		Event(Event&& other) noexcept = default;
		auto operator=(Event&& other) noexcept -> Event & = default;

		// Custom constructors
		//	Constructor for a named or anonymous Event object.
		Event(const EventCreateParams& params = {})
			: m_name{ params.Name }
		{
			InternalCreate(params.IsSignaled, params.IsInheritable);
		}

		Event(const EventOpenParams& params)
			: m_name{ params.Name }
		{
			if (not m_name or m_name->empty())
				throw Error::RuntimeError{ "Name of Event to open cannot be empty" };

			m_event = Win32::OpenEventW(params.DesiredAccess, params.IsInheritable, m_name->c_str());
			if (not m_event)
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to create or open event" };
		}

		constexpr operator Win32::HANDLE() const noexcept
		{
			return m_event.get();
		}

		constexpr operator bool() const noexcept
		{
			return m_event != nullptr;
		}

		void Signal()
		{
			if (not m_event)
				throw Error::RuntimeError{ "No Event to signal" };
			if (not Win32::SetEvent(m_event.get()))
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to signal event" };
		}

		auto TrySignal() noexcept -> std::expected<void, std::exception>
			try
			{
				Signal();
				return {};
			}
			catch (const std::exception& ex)
			{
				return std::unexpected(ex);
			}

			void Reset() requires VIsManualReset
			{
				if (not m_event)
					return;
				if (not Win32::ResetEvent(m_event.GetHandle()))
					throw Error::Win32Error{ Win32::GetLastError(), "ResetEvent() failed" };
			}

			auto TryReset() noexcept -> std::expected<void, std::string>
				requires VIsManualReset
			try
			{
				Reset();
				return {};
			}
			catch (const std::exception& ex)
			{
				return std::unexpected(ex.what());
			}

			void WaitOnEvent() const
			{
				if (not m_event)
					throw Error::RuntimeError{ "No Event to wait on" };

				auto status = static_cast<Win32::WaitResult>(Win32::WaitForSingleObject(m_event.get(), Win32::Infinite));
				if (status == Win32::WaitResult::Failed)
					throw Error::Win32Error{ Win32::GetLastError(), "WaitForSingleObject failed" };
				if (status == Win32::WaitResult::Abandoned)
					throw Error::RuntimeError{ "The wait was abandoned" };
			}

			auto WaitOnEvent(Win32::DWORD millis, bool interruptible) const -> bool
			{
				return WaitOnEvent(std::chrono::milliseconds(millis), interruptible);
			}

			auto WaitOnEvent(Concepts::Duration auto&& time, bool alertable) const -> bool
			{
				using std::chrono::duration_cast;
				using std::chrono::milliseconds;

				if (not m_event)
					throw Error::RuntimeError{ "No Event to wait on" };

				auto millis = static_cast<unsigned long>(duration_cast<milliseconds>(time).count());
				switch (Win32::WaitResult status = static_cast<Win32::WaitResult>(Win32::WaitForSingleObjectEx(m_event.GetHandle(), millis, alertable)))
				{
				case Win32::WaitResult::Success:
					return true;
				case Win32::WaitResult::Timeout:
					return false;
				case Win32::WaitResult::Abandoned:
					throw Error::RuntimeError{ "The wait was abandoned" };
				case Win32::WaitResult::Failed:
					throw Error::Win32Error{ Win32::GetLastError(), "WaitForSingleObject() failed" };
				}
				return false;
			}

			auto TryWaitOnEvent(
				Concepts::Duration auto&& time,
				bool alertable
			) const noexcept -> bool
				try
				{
					//https://codeyarns.com/tech/2018-08-22-how-to-get-function-name-in-c.html
					WaitOnEvent(time, alertable);
					return true;
				}
				catch (const std::exception& ex)
				{
					return false;
				}

				constexpr auto Detach() noexcept -> Win32::HANDLE
				{
					return m_event.release();
				}

				constexpr auto GetHandle() const noexcept -> Win32::HANDLE
				{
					return m_event.get();
				}

				constexpr void Close()
				{
					m_event = nullptr;
				}

				constexpr auto GetName() const noexcept -> std::optional<std::wstring>
				{
					return m_name;
				}

				constexpr auto IsManualReset() const noexcept -> bool
				{
					return VIsManualReset;
				}

				auto IsSignalled() const -> bool
				{
					return WaitOnEvent(std::chrono::milliseconds{ 0 }, false);
				}

	private:
		void InternalCreate(bool isSignaled, bool isInheritable)
		{
			m_event = Raii::HandleUniquePtr{
				Win32::CreateEventW(
					nullptr,			// security attributes
					VIsManualReset,	// manual reset event
					isSignaled,			// is initially signalled
					m_name and not m_name->empty() ? m_name->c_str() : nullptr // name
				) };
			if (not m_event)
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to create or open event" };
			Raii::Handle::SetInheritance(m_event.get(), isInheritable);
		}

		Raii::HandleUniquePtr m_event;
		std::optional<std::wstring> m_name;
	};

	template<typename T>
	struct IsAnyEventT : std::false_type {};
	template<bool VIsManualReset>
	struct IsAnyEventT<Event<VIsManualReset>> : std::true_type {};
	template<typename T>
	constexpr bool IsAnyEventV = IsAnyEventT<T>::value;
	template<typename T>
	concept AnyEvent = IsAnyEventV<T>;

	using ManualResetEvent = Event<true>;
	using AutoResetEvent = Event<false>;
}