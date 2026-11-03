export module Shared:Async.Overlapped;
import std;
import Win32;
import :Error;
import :Raii;

export namespace Async
{
	template<typename T>
	concept OverlappedCallable = requires(T t, Win32::HANDLE h, Win32::OVERLAPPED * o)
	{
		{ t(h, o) } -> std::convertible_to<bool>;
	};
	class OverlappedOperation
	{
	public:
		~OverlappedOperation()
		{
			if (InProgress())
			{
				auto transferred = Win32::DWORD{};
				Win32::GetOverlappedResult(m_file.get(), &m_overlapped, &transferred, true);
			}
		}

		explicit OverlappedOperation(Win32::HANDLE file)
			: m_event{ Win32::CreateEventW(nullptr, true, false, nullptr) },
			m_overlapped{ .hEvent = m_event.get() }
		{
			if (not m_event)
				throw Error::Win32Error{ Win32::GetLastError(), "CreateEventW() failed" };
			if (file == nullptr or file == Win32::InvalidHandleValue)
				throw Error::RuntimeError{ "Invalid file handle" };
			m_file = Raii::Win32Handle::DuplicateHandle(file);
		}

		explicit OverlappedOperation(Win32::HANDLE file, OverlappedCallable auto&& operation)
			: OverlappedOperation(file)
		{
			Start(std::forward<decltype(operation)>(operation));
		}

		OverlappedOperation(const OverlappedOperation&) = delete;
		OverlappedOperation& operator=(const OverlappedOperation&) = delete;

		void Start(OverlappedCallable auto&& operation)
		{
			if (m_started)
				throw Error::RuntimeError{ "Overlapped operation has already been started" };

			m_started = true;
			m_error = Win32::SystemCodes::IoPending;
			auto succeeded = std::invoke(std::forward<decltype(operation)>(operation), m_file.get(), &m_overlapped);
			m_error = succeeded ? Win32::SystemCodes::NoError : Win32::GetLastError();
		}

		auto InProgress() const noexcept -> bool
		{
			UpdateResult();
			return m_started and m_error == Win32::SystemCodes::IoPending;
		}

		auto Finished() const noexcept -> bool
		{
			return m_started and not InProgress();
		}

		auto Success() const noexcept -> bool
		{
			return Finished() and m_error == Win32::SystemCodes::NoError;
		}

		auto GetError() const noexcept -> Win32::DWORD
		{
			return Finished() ? m_error : Win32::SystemCodes::NoError;
		}

		auto WaitForCompletion(Win32::DWORD timeoutMilliseconds = Win32::Infinite) -> bool
		{
			if (not m_started)
				throw Error::RuntimeError{ "Overlapped operation has not been started" };
			auto waitResult = Win32::WaitForSingleObjectEx(m_event.get(), timeoutMilliseconds, false);
			if (waitResult == Win32::WaitResult::Failed)
				throw Error::Win32Error{ Win32::GetLastError(), "WaitForSingleObjectEx() failed" };
			return waitResult == Win32::WaitResult::Success;
		}

		auto WaitForCompletion(
			bool alertable,
			Win32::DWORD timeoutMilliseconds
		) -> bool
		{
			if (not m_started)
				throw Error::RuntimeError{ "Overlapped operation has not been started" };

			auto started = std::chrono::steady_clock::now();
			auto remaining = timeoutMilliseconds;
			while (InProgress())
			{
				auto waitResult = Win32::WaitForSingleObjectEx(m_event.get(), remaining, alertable);
				if (waitResult == Win32::WaitResult::Failed)
					throw Error::Win32Error{ Win32::GetLastError(), "WaitForSingleObjectEx() failed" };
				if (waitResult == Win32::WaitResult::Timeout)
					return Finished();

				if (timeoutMilliseconds != Win32::Infinite)
				{
					auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::steady_clock::now() - started).count();
					if (elapsed >= timeoutMilliseconds)
						return Finished();
					remaining = timeoutMilliseconds - static_cast<Win32::DWORD>(elapsed);
				}
			}
			return true;
		}

		auto BytesReadOrWritten() const -> Win32::DWORD
		{
			if (not Finished())
				throw Error::RuntimeError{ "Overlapped operation has not finished" };
			auto transferred = Win32::DWORD{};
			if (not Win32::GetOverlappedResult(m_file.get(), &m_overlapped, &transferred, false))
				throw Error::Win32Error{ Win32::GetLastError(), "GetOverlappedResult() failed" };
			return transferred;
		}

		auto GetEventHandle() const noexcept -> Win32::HANDLE
		{
			return m_event.get();
		}

	private:
		void UpdateResult() const noexcept
		{
			if (not m_started or m_error != Win32::SystemCodes::IoPending)
				return;

			auto transferred = Win32::DWORD{};
			if (Win32::GetOverlappedResult(m_file.get(), &m_overlapped, &transferred, false))
				m_error = Win32::SystemCodes::NoError;
			else if (auto error = Win32::GetLastError(); error != Win32::SystemCodes::IoIncomplete)
				m_error = error;
		}

		Raii::Win32Handle m_file;
		Raii::HandleUniquePtr m_event;
		mutable Win32::OVERLAPPED m_overlapped;
		bool m_started = false;
		mutable Win32::DWORD m_error = Win32::SystemCodes::NoError;
	};

	struct BufferAndOverlapped
	{
		BufferAndOverlapped() = default;
		BufferAndOverlapped(const BufferAndOverlapped&) = delete;
		auto operator=(const BufferAndOverlapped&) -> BufferAndOverlapped & = delete;
		BufferAndOverlapped(BufferAndOverlapped&&) = default;
		auto operator=(BufferAndOverlapped&&) -> BufferAndOverlapped & = default;

		std::vector<std::byte> Buffer;
		std::unique_ptr<Async::OverlappedOperation> Overlapped;

		template<typename T>
		auto BufferAs() -> T
		{
			return T{ Buffer.begin(), Buffer.end() };
		}

		template<>
		auto BufferAs<std::string>() -> std::string
		{
			return std::string{
				reinterpret_cast<const char*>(Buffer.data()),
				reinterpret_cast<const char*>(Buffer.data() + Buffer.size()) };
		}

		template<>
		auto BufferAs<std::wstring>() -> std::wstring
		{
			return std::wstring{
				reinterpret_cast<const wchar_t*>(Buffer.data()),
				reinterpret_cast<const wchar_t*>(Buffer.data() + Buffer.size()) };
		}
	};
}

