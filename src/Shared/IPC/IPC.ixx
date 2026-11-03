export module Shared:IPC;
import std;
import Win32;
import :Error;
import :Raii;
import :Util;
import :Async;

// Pipe creation
export namespace IPC
{
	struct ValidateResult
	{
		bool IsValid = false;
		std::optional<std::string> Reason;
		constexpr operator bool() const noexcept { return IsValid; }
	};

	struct PipeServerDescription
	{
		std::wstring Name;
		Win32::DWORD OpenMode = 0;
		Win32::DWORD PipeMode = 0;
		Win32::DWORD MaxInstances = 0;
		Win32::DWORD OutBufferSize = 0;
		Win32::DWORD InBufferSize = 0;
		Win32::DWORD DefaultTimeout = 0;
		std::optional<Win32::SECURITY_ATTRIBUTES> SecurityAttributes;

		void Validate() const
		{
			if (auto isValid = IsValid(); not isValid)
				throw Error::RuntimeError{ isValid.Reason.value_or("PipeServerDescription is invalid") };
		}

		auto IsValid() const noexcept -> ValidateResult
		{
			if (Name.empty())
				return { .Reason = "Pipe name cannot be empty" };
			if (OpenMode == 0)
				return { .Reason = "Pipe open mode cannot be zero" };
			if (MaxInstances == 0 or MaxInstances > Win32::UnlimitedPipeInstances)
				return { .Reason = std::format("Pipe max instances must be between 1 and {}", Win32::UnlimitedPipeInstances) };
			if (OutBufferSize == 0)
				return { .Reason = "Pipe output buffer size cannot be zero" };
			if (InBufferSize == 0)
				return { .Reason = "Pipe input buffer size cannot be zero" };

			return { .IsValid = true };
		}
	};

	struct PipeClientDescription
	{
		std::wstring Name;
		Win32::DWORD DesiredAccess = 0;
		std::optional<Win32::SECURITY_ATTRIBUTES> SecurityAttributes;
		Win32::DWORD FlagsAndAttributes = 0;
		Win32::DWORD ConnectTimeout = 0;

		void Validate() const
		{
			if (auto isValid = IsValid(); not isValid)
				throw Error::RuntimeError{ isValid.Reason.value_or("PipeClientDescription is invalid") };
		}

		auto IsValid() const noexcept -> ValidateResult
		{
			if (Name.empty())
				return { .Reason = "Pipe name cannot be empty" };
			if (DesiredAccess == 0)
				return { .Reason = "Pipe desired access cannot be zero" };
			return { .IsValid = true };
		}
	};
}

// Pipe implementation
export namespace IPC
{
	template<bool Readable, bool Writable, bool Async, bool IsServer>
	class Pipe
	{
	public:
		~Pipe()
		{
			TryClose();
		}

		Pipe(const Pipe&) = delete;
		auto operator=(const Pipe&) -> Pipe & = delete;

		Pipe(Pipe&& other) noexcept
			: m_handle{ std::move(other.m_handle) }
		{}
		auto operator=(Pipe&& other) noexcept -> Pipe&
		{
			if (this != &other)
				m_handle = std::move(other.m_handle);
			return *this;
		}

		Pipe(Raii::Win32Handle&& handle)
			: m_handle{ std::move(handle) }
		{}
		Pipe(const Raii::Win32Handle& handle)
			: m_handle{ handle }
		{}

		Pipe(const IPC::PipeServerDescription& desc)
			requires IsServer
		{
			desc.Validate();
			auto sa = desc.SecurityAttributes
				? const_cast<Win32::SECURITY_ATTRIBUTES*>(std::addressof(*desc.SecurityAttributes))
				: static_cast<Win32::SECURITY_ATTRIBUTES*>(nullptr);
			auto pipeHandle =
				Win32::CreateNamedPipeW(
					desc.Name.c_str(), // name
					desc.OpenMode, // open mode
					desc.PipeMode, // pipe mode
					desc.MaxInstances, // max instances
					desc.OutBufferSize, // out buffer size
					desc.InBufferSize, // in buffer size
					desc.DefaultTimeout, // default timeout, a value of zero will result in a default time-out of 50 milliseconds.
					sa // security attributes
				);
			if (pipeHandle == Win32::InvalidHandleValue)
				throw Error::Win32Error{ Win32::GetLastError() };
			m_handle = Raii::Win32Handle{ pipeHandle };
		}

		Pipe(const IPC::PipeClientDescription& desc)
			requires (not IsServer)
		{
			desc.Validate();
			// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew

			auto handle = Win32::HANDLE{ Win32::InvalidHandleValue };
			while (handle == Win32::InvalidHandleValue)
			{
				handle = Win32::CreateFileW(
					desc.Name.c_str(),
					desc.DesiredAccess,
					0, // share mode, not relevant for named pipes
					desc.SecurityAttributes
					? const_cast<Win32::SECURITY_ATTRIBUTES*>(std::addressof(*desc.SecurityAttributes))
					: nullptr,
					static_cast<Win32::DWORD>(Win32::OpenExisting), // creation disposition, only open existing pipes is valid
					desc.FlagsAndAttributes,
					nullptr // template file, not relevant for named pipes
				);
				if (handle == Win32::InvalidHandleValue)
				{
					if (auto lastError = Win32::GetLastError(); lastError != Win32::SystemCodes::PipeBusy or desc.ConnectTimeout == 0)
						throw Error::Win32Error{ lastError, "Failed to connect client pipe" };
					if (not Win32::WaitNamedPipeW(desc.Name.c_str(), desc.ConnectTimeout))
						throw Error::Win32Error{ Win32::GetLastError(), "Timed out trying to connect client pipe" };
				}
			}
			m_handle = Raii::Win32Handle{ handle };
		}

		void Flush()
			requires Writable
		{
			if (m_handle and not Win32::FlushFileBuffers(m_handle.get()))
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to flush pipe buffers" };
		}

#pragma region Shared methods
		void Close()
		{
			if constexpr (IsServer)
				Disconnect();
			m_handle.Close();
		}

		void TryClose()
		{
			if constexpr (IsServer)
				TryDisconnect();
			m_handle.Close();
		}

		[[nodiscard]]
		auto GetHandle() const noexcept -> Util::BorrowedHandle
		{
			return m_handle.get();
		}

		[[nodiscard]]
		auto Write(std::ranges::sized_range auto&& buffer) -> std::size_t
			requires (Writable and not Async)
		{
			if (not m_handle)
				throw Error::RuntimeError{ "Pipe handle is not valid" };
			auto bytesWritten = Win32::DWORD{};
			auto succeeded = Win32::WriteFile(
				m_handle.get(),
				std::ranges::data(buffer),
				static_cast<Win32::DWORD>(std::ranges::size(buffer)),
				std::addressof(bytesWritten),
				nullptr
			);
			if (not succeeded)
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to write to pipe" };
			return static_cast<std::size_t>(bytesWritten);
		}

		[[nodiscard]]
		auto WriteAsync(std::ranges::sized_range auto&& buffer) -> std::unique_ptr<Async::OverlappedOperation>
			requires (Writable and Async)
		{
			if (not m_handle)
				throw Error::RuntimeError{ "Pipe handle is not valid" };

			return std::make_unique<Async::OverlappedOperation>(
				m_handle.get(),
				[&](Win32::HANDLE h, Win32::OVERLAPPED* o) -> bool
				{
					auto bytesWritten = Win32::DWORD{};
					auto succeeded = Win32::WriteFile(
						h,
						std::ranges::data(buffer),
						static_cast<Win32::DWORD>(std::ranges::size(buffer)),
						std::addressof(bytesWritten),
						o
					);
					if (not succeeded)
						throw Error::Win32Error{ Win32::GetLastError(), "Failed to write to pipe" };
					return true;
				}
			);
		}

		[[nodiscard]]
		auto Read(std::ranges::sized_range auto&& buffer) -> std::size_t
			requires (Readable and not Async)
		{
			if (not m_handle)
				throw Error::RuntimeError{ "Pipe handle is not valid" };

			auto bytesRead = Win32::DWORD{};
			auto succeeded = Win32::ReadFile(
				m_handle.get(),
				std::ranges::data(buffer),
				static_cast<Win32::DWORD>(std::ranges::size(buffer)),
				std::addressof(bytesRead),
				Async ? nullptr : nullptr
			);
			if (not succeeded)
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to read from pipe" };
			return static_cast<std::size_t>(bytesRead);
		}

		[[nodiscard]]
		auto ReadAsync(std::ranges::sized_range auto&& buffer) -> std::unique_ptr<Async::OverlappedOperation>
			requires (Readable and Async)
		{
			if (not m_handle)
				throw Error::RuntimeError{ "Pipe handle is not valid" };

			return std::make_unique<Async::OverlappedOperation>(
				m_handle.get(),
				[&](Win32::HANDLE h, Win32::OVERLAPPED* o) -> bool
				{
					auto bytesRead = Win32::DWORD{};
					auto succeeded = Win32::ReadFile(
						h,
						std::ranges::data(buffer),
						static_cast<Win32::DWORD>(std::ranges::size(buffer)),
						std::addressof(bytesRead),
						o
					);
					if (not succeeded and Win32::GetLastError() != Win32::SystemCodes::IoPending)
						throw Error::Win32Error{ Win32::GetLastError(), "Failed to read from pipe" };
					return true;
				}
			);
		}

		[[nodiscard]]
		auto ReadAsync(Win32::DWORD inBufferSize) -> Async::BufferAndOverlapped
			requires (Readable and Async)
		{
			if (not m_handle)
				throw Error::RuntimeError{ "Pipe handle is not valid" };
			auto buffer = Async::BufferAndOverlapped{};
			buffer.Buffer.resize(inBufferSize);
			buffer.Overlapped = std::make_unique<Async::OverlappedOperation>(
				m_handle.get(),
				[&](Win32::HANDLE h, Win32::OVERLAPPED* o) -> bool
				{
					auto bytesRead = Win32::DWORD{};
					auto succeeded = Win32::ReadFile(
						h,
						buffer.Buffer.data(),
						static_cast<Win32::DWORD>(buffer.Buffer.size()),
						std::addressof(bytesRead),
						o
					);
					if (not succeeded and Win32::GetLastError() != Win32::SystemCodes::IoPending)
						throw Error::Win32Error{ Win32::GetLastError(), "Failed to read from pipe" };
					return true;
				}
			);

			return buffer;
		}
#pragma endregion

#pragma region Server methods
		void Disconnect() noexcept
			requires IsServer
		{
			if (m_handle and not Win32::DisconnectNamedPipe(m_handle.get()))
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to disconnect named pipe" };
		}

		auto TryDisconnect() noexcept -> bool
			requires IsServer
		{
			if (not m_handle)
				return false;
			return Win32::DisconnectNamedPipe(m_handle.get()) != 0;
		}

		void Connect()
			requires (IsServer and not Async)
		{
			if (not m_handle)
				throw Error::RuntimeError{ "Pipe handle is not valid" };

			auto succeeded = Win32::ConnectNamedPipe(m_handle.get(), nullptr);
			if (not succeeded)
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to connect named pipe" };
		}

		[[nodiscard]]
		auto ConnectAsync() -> std::unique_ptr<Async::OverlappedOperation>
			requires (IsServer and Async)
		{
			if (not m_handle)
				throw Error::RuntimeError{ "Pipe handle is not valid" };

			return std::make_unique<Async::OverlappedOperation>(
				m_handle.get(),
				[&](Win32::HANDLE h, Win32::OVERLAPPED* o) -> bool
				{
					auto succeeded = Win32::ConnectNamedPipe(h, o);
					if (succeeded)
						return true;

					switch (auto lastError = (Win32::GetLastError()); lastError)
					{
						case Win32::SystemCodes::IoPending:
						case Win32::SystemCodes::PipeConnected:
							return true;
						default:
							throw Error::Win32Error{ lastError, "Failed to connect named pipe" };
					};
				}
			);
		}
#pragma endregion

	protected:
		Raii::Win32Handle m_handle;
	};
}

// Pipe type aliases
export namespace  IPC
{
	template<bool Readable, bool Writable>
	using AsyncServerPipe = Pipe<Readable, Writable, true, true>;
	using AsyncDuplexServerPipe = Pipe<true, true, true, true>;

	template<bool Readable, bool Writable>
	using AsyncClientPipe = Pipe<Readable, Writable, true, false>;
	using AsyncDuplexClientPipe = Pipe<true, true, true, false>;
}
