#pragma once
#include <Windows.h>
#include <string>

namespace Win32Utils::Async
{
	/// <summary>
	///		Represents a Win32 mutex, an object used for interprocess
	///		synchronisation and communication.
	/// </summary>
	class Mutex
	{
		public:
			/// <summary>
			/// Default constructor. Does not initialise any underlying mutex.
			/// </summary>
			Mutex();

			/// <summary>
			///		Creates a new named or anonymous mutex, or opens an existing named one.
			/// </summary>
			/// <param name="name">
			///		The name of the mutex to create or open. Pass an empty
			///		string to create an anonymous mutex.
			/// </param>
			/// <param name="createNew">
			///		Whether to create a new mutex or open an existing one.
			/// </param>
			/// <param name="acquireOnCreation">
			///		Whether to acquire the mutex if it's being created.
			///		If the createNew parameter is false, this parameter is ignored.
			/// </param>
			/// <param name="inheritable">
			///		Whether the handle can be inherited by child processes.
			/// </param>
			Mutex(
				const std::wstring_view name,
				const bool createNew,
				const bool acquireOnCreation,
				const bool inheritable
			);
			
			/// <summary>
			///		Creates an anonymous mutex.
			/// </summary>
			/// <param name="acquire">
			///		Whether to request acquisition of the mutex.
			/// </param>
			/// <param name="inheritable">
			///		Whether the mutex is inheritable to child processes.
			/// </param>
			/// <exception cref="std::runtime_error">
			///		Thrown when mutex creation failed.
			/// </exception>
			Mutex(const bool acquire, const bool inheritable);

			/// <summary>
			///		Clones a mutex.
			/// </summary>
			/// <param name="other"></param>
			Mutex(const Mutex& other);

			/// <summary>
			///		Move constructor.
			/// </summary>
			/// <param name="other">The rvalue to move.</param>
			Mutex(Mutex&& other) noexcept;

			/// <summary>
			///		Destroys this mutex.
			/// </summary>
			virtual ~Mutex();

			/// <summary>
			///		Duplicates the mutex handle held by the RHS operand.
			/// </summary>
			/// <param name="other">
			///		The mutex whose handle needs to be duplicated.
			///		The handle owned by this mutex must not be null.
			/// </param>
			/// <exception cref="std::runtime_error">
			///		Thrown when the RHS operand is in an invalid state,
			///		or if the duplicatation failed.
			/// </exception>
			virtual void operator=(const Mutex& other);

			/// <summary>
			///		Move assignment.
			/// </summary>
			/// <param name="other">The mutex to move.</param>
			virtual void operator=(Mutex&& other) noexcept;

			/// <summary>
			///		Blocks the current thread for a specified amount of time 
			///		(or indefinitely) until the mutex is acquired.
			/// </summary>
			/// <param name="waitTime">
			///		The time in milliseconds to wait to acquire the mutex.
			///		Pass INFINITE to wait indefinitely.
			/// </param>
			/// <returns>
			///		Returns true if the mutex was successfully acquired,
			///		or false if the timeout occurred.
			/// </returns>
			/// <exception cref="std::runtime_error">
			///		Failed to acquire the mutex for reasons other than the
			///		timeout was reached.
			/// </exception>
			virtual bool Lock(const DWORD waitTime);

			/// <summary>
			///		Frees the mutex, allowing another process to acquire it.
			/// <exception cref="std::runtime_error">
			///		Failed to release the mutex.
			/// </exception>
			/// </summary>
			virtual void Unlock();

		protected:
			virtual void Cleanup();
			virtual void Move(Mutex& other) noexcept;

		protected:
			std::wstring m_name;
			bool m_inheritable;
			bool m_created;
			bool m_locked;
			HANDLE m_mutex;
	};
}