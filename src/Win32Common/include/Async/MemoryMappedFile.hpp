#pragma once
#include <Windows.h>
#include <string>

namespace Win32Utils::Async
{
	/// <summary>
	///		Represents a <a href="https://docs.microsoft.com/en-us/dotnet/standard/io/memory-mapped-files">Win32 memory-mapped file</a>,
	///		which allows processes to share memory. This is a copyable
	///		and movable object.
	/// </summary>
	class MemoryMappedFile
	{
		// Constructors and destructor
		public:
			/// <summary>
			///		Unmaps the view of the MemoryMappedFile and releases
			///		the handle owned by this object.
			/// </summary>
			virtual ~MemoryMappedFile();

			MemoryMappedFile();

			/// <summary>
			///		Creates or opens a new memory mapped file.
			/// </summary>
			/// <param name="name">
			///		The name of the memory mapped file to create or open.
			/// </param>
			/// <param name="maxSize">
			///		The maximum size of the memory mapped file.
			/// </param>
			/// <param name="createFile">
			///		Whether to create a new memory mapped file or open an
			///		existing one that has been inherited.
			/// </param>
			/// <param name="inheritable">
			///		Whether the acquired handle can be inherited by child processes.
			/// </param>
			MemoryMappedFile(
				const std::wstring& name,
				const UINT maxSize,
				const bool createFile,
				const bool inheritable
			);

			/// <summary>
			///		Duplicates the specified MemoryMappedFile.
			/// </summary>
			/// <param name="other">The MemoryMappedFile to duplicate.</param>
			MemoryMappedFile(const MemoryMappedFile& other);

			/// <summary>
			///		Unmaps the view of the MemoryMappedFile and releases
			///		the handle owned by this object before duplicating
			///		from the specified MemoryMappedFile.
			/// </summary>
			/// <param name="other">The MemoryMappedFile to duplicate.</param>
			virtual void operator=(const MemoryMappedFile& other);

			MemoryMappedFile(MemoryMappedFile&& other) noexcept;
			virtual void operator=(MemoryMappedFile&& other) noexcept;

		// API
		public:
			/// <summary>
			///		Gets the view pointer of the memory mapped file.
			/// </summary>
			/// <returns>The view object.</returns>
			virtual void* GetViewPointer();

		protected:
			/// <summary>
			///		Unlocks the mutex if it is currently owned, releases the 
			///		mutex handle, and clears the underlying pointer.
			/// </summary>
			virtual void Cleanup();

			virtual void Move(MemoryMappedFile& other) noexcept;

			/// <summary>
			///		Duplicates the specified MemoryMappedFile.
			/// </summary>
			/// <param name="other">The MemoryMappedFile to duplicate.</param>
			virtual void Duplicate(const MemoryMappedFile& other);

		protected:
			bool m_inheritable;
			std::wstring m_mmfName;
			UINT m_maxSize;
			HANDLE m_mapFile;
			void* m_view;
	};
}