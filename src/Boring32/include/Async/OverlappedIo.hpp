#pragma once
#include "Event.hpp"

namespace Boring32::Async
{
	class OverlappedIo
	{
		public:
			virtual ~OverlappedIo();
			OverlappedIo();
			OverlappedIo(
				const bool createOrOpen,
				const bool isInheritable,
				const bool manualReset,
				const bool isSignaled, 
				const std::wstring name
			);

			OverlappedIo(const OverlappedIo& other);
			virtual void operator=(const OverlappedIo& other);

			OverlappedIo(OverlappedIo&& other) noexcept;
			virtual void operator=(OverlappedIo&& other) noexcept;

			virtual DWORD GetBytesTransferred(const bool wait);

		public:
			Event IoEvent;
			Raii::Win32Handle IoHandle;
			OVERLAPPED IoOverlapped;
			bool CallReturnValue;
			DWORD LastErrorValue;

		protected:
			virtual void Copy(const OverlappedIo& other);
			virtual void Move(OverlappedIo& other) noexcept;
	};
}
