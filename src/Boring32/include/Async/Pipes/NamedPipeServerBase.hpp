#pragma once
#include <string>
#include <memory>
#include "../../Raii/Raii.hpp"

namespace Boring32::Async
{
	class NamedPipeServerBase
	{
		public:
			virtual ~NamedPipeServerBase();
			NamedPipeServerBase();
			NamedPipeServerBase(
				const std::wstring& pipeName, 
				const DWORD size,
				const DWORD maxInstances, // PIPE_UNLIMITED_INSTANCES
				const std::wstring& sid,
				const bool isInheritable,
				const bool isLocalPipe
			);
			NamedPipeServerBase(
				const std::wstring& pipeName,
				const DWORD size,
				const DWORD maxInstances, // PIPE_UNLIMITED_INSTANCES
				const std::wstring& sid,
				const bool isInheritable,
				const DWORD openMode,
				const DWORD pipeMode
			);

			NamedPipeServerBase(const NamedPipeServerBase& other);
			virtual void operator=(const NamedPipeServerBase& other);

			NamedPipeServerBase(NamedPipeServerBase&& other) noexcept;
			virtual void operator=(NamedPipeServerBase&& other) noexcept;

		public:
			virtual void Close();
			virtual void Disconnect();
			virtual Raii::Win32Handle& GetInternalHandle();
			virtual std::wstring GetName() const;
			virtual DWORD GetSize() const;
			virtual DWORD GetMaxInstances() const;
			virtual DWORD GetPipeMode() const;
			virtual DWORD GetOpenMode() const;
			virtual DWORD UnreadCharactersRemaining() const;

		protected:
			virtual void InternalCreatePipe();
			virtual void Copy(const NamedPipeServerBase& other);
			virtual void Move(NamedPipeServerBase& other) noexcept;

		protected:
			Raii::Win32Handle m_pipe;
			std::wstring m_pipeName;
			std::wstring m_sid;
			DWORD m_size;
			DWORD m_maxInstances;
			bool m_isInheritable;
			DWORD m_pipeMode;
			DWORD m_openMode;
	};
}
