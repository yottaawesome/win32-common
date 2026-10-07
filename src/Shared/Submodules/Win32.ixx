module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <comdef.h>

export module Win32;
import PlatformShared;

export namespace Win32
{
	constexpr auto EventModifyState = EVENT_MODIFY_STATE;
	constexpr auto Synchronize = SYNCHRONIZE;
	constexpr auto MaxPath = MAX_PATH;
	using
		::HINSTANCE,
		::LPWSTR,
		::DWORD,
		::LRESULT,
		::HWND,
		::UINT,
		::WPARAM,
		::LPARAM,
		::HICON,
		::WNDCLASSW,
		::HRESULT,
		::LONG,
		::HANDLE,
		::SIZE,
		::LONG_PTR,
		::HWND,
		::BYTE,
		::LPSTR,
		::HBRUSH,
		::BOOL,
		::HCURSOR,
		::WNDCLASSEXW,
		::GUID,
		::CREATESTRUCT,
		::LARGE_INTEGER,
		::RECT,
		::MSG,
		::COINIT,
		::PROPVARIANT,
		::VARENUM,
		::IUnknown,
		::CREATEFILE2_EXTENDED_PARAMETERS,
		::FILE_STANDARD_INFO,
		::FILE_INFO_BY_HANDLE_CLASS,
		::CLSCTX,
		::USHORT,
		::WCHAR,
		::CoUninitialize,
		::CoInitializeEx,
		::SetWindowPos,
		::GetSystemMetrics,
		::wcsncpy_s,
		::GetClientRect,
		::wcscpy_s,
		::wcscat_s,
		::wcsrchr,
		::wcslen,
		::OutputDebugStringW,
		::ReadFile,
		::GetFileInformationByHandleEx,
		::CreateFile2,
		::CoTaskMemFree,
		::SetEvent,
		::ResetEvent,
		::CreateEventExW,
		::CloseHandle,
		::MessageBoxA,
		::MessageBoxW,
		::QueryPerformanceCounter,
		::QueryPerformanceFrequency,
		::ShowWindow,
		::TranslateMessage,
		::GetStdHandle,
		::MultiByteToWideChar,
		::FreeConsole,
		::AllocConsole,
		::GetConsoleMode,
		::SetConsoleMode,
		::CoCreateInstance,
		::DispatchMessageW,
		::DestroyWindow,
		::PostQuitMessage,
		::GetWindowLongPtr,
		::GetDpiForWindow,
		::GetDpiForSystem,
		::SetWindowLongPtr,
		::PeekMessageA,
		::PeekMessageW,
		::GetLastError,
		::FormatMessageA,
		::FormatMessageW,
		::mbstowcs_s,
		::swprintf_s,
		::LocalFree,
		::_stricmp,
		::_aligned_free,
		::WideCharToMultiByte,
		::_aligned_malloc,
		::SetFilePointerEx,
		::GetModuleHandleW,
		::LoadIconW,
		::LoadCursorW,
		::GetStockObject,
		::RegisterClassExW,
		::DefWindowProcW,
		::RegisterClassW,
		::CreateWindowExW
		;

	constexpr auto Truncate = _TRUNCATE;

	namespace SystemMetrics
	{
		enum
		{
			ScreenWidth = SM_CXSCREEN,
			ScreenHeight = SM_CYSCREEN
		};
	}

	namespace CodePage
	{
		enum
		{
			Utf8 = CP_UTF8,
			Ansi = CP_ACP
		};
	}

	namespace WideCharFlags
	{
		enum
		{
			NoBestFitChars = WC_NO_BEST_FIT_CHARS
		};
	}

	constexpr auto GuidNull = PlatformShared::InvocableValue<[] {return GUID_NULL; } > {};
	constexpr auto EnableVirtualTerminalProcessing = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	constexpr auto StdOutputHandle = STD_OUTPUT_HANDLE;

	constexpr auto FileShareRead = FILE_SHARE_READ;
	constexpr auto FileShareWrite = FILE_SHARE_WRITE;
	constexpr auto FileShareDelete = FILE_SHARE_DELETE;
	constexpr auto OpenExisting = OPEN_EXISTING;

	constexpr auto FileAttributeNormal = FILE_ATTRIBUTE_NORMAL;
	constexpr auto FileFlagSequentialScan = FILE_FLAG_SEQUENTIAL_SCAN;
	constexpr auto SecurityQosAnonymous = SECURITY_ANONYMOUS;
	constexpr auto FileBegin = FILE_BEGIN;
	constexpr auto InvalidHandleValue = PlatformShared::ConstexprValue<INVALID_HANDLE_VALUE>{};

	namespace HrCodes
	{
		constexpr auto Ok = S_OK;
		constexpr auto False = S_FALSE;
		constexpr auto Fail = E_FAIL;
		constexpr auto OutOfMemory = E_OUTOFMEMORY;
		constexpr auto NoInterface = E_NOINTERFACE;
	}

	namespace AccessRights
	{
		constexpr auto GenericRead = GENERIC_READ;
		constexpr auto EventModifyState = EVENT_MODIFY_STATE;
		constexpr auto Synchronize = SYNCHRONIZE;
	}

	namespace Brushes
	{
		constexpr auto White = WHITE_BRUSH;
	}

	namespace MessageBoxFlags
	{
		enum
		{
			Ok = MB_OK,
			IconError = MB_ICONERROR
		};
	}

	namespace SetWindowPosFlags
	{
		enum
		{
			NoZOrder = SWP_NOZORDER,
			NoActivate = SWP_NOACTIVATE
		};
	}

	constexpr auto LoWord(auto dw) noexcept -> auto
	{
		return LOWORD(dw);
	}

	constexpr auto HiWord(auto dw) noexcept -> auto
	{
		return HIWORD(dw);
	}

	void ZeroAllMemory(void* ptr, std::size_t size) noexcept
	{
		std::memset(ptr, 0, size);
	}

	constexpr auto HrFacility(Win32::HRESULT hr) noexcept -> long
	{
		return HRESULT_FACILITY(hr);
	}
	constexpr auto HrCode(Win32::HRESULT hr) noexcept -> long
	{
		return HRESULT_CODE(hr);
	}
	constexpr auto HrSeverity(Win32::HRESULT hr) noexcept -> long
	{
		return HRESULT_SEVERITY(hr);
	}
	constexpr auto HrResult(Win32::HRESULT hr) noexcept -> long
	{
		return HRESULT_CODE(hr);
	}
	constexpr auto Succeeded(Win32::HRESULT hr) noexcept -> bool
	{
		return SUCCEEDED(hr);
	}
	constexpr auto Failed(Win32::HRESULT hr) noexcept -> bool
	{
		return FAILED(hr);
	}
	constexpr auto FacilityWindows = FACILITY_WINDOWS;

	constexpr auto IdiApplication = PlatformShared::ConstexprValue<IDI_APPLICATION>{};
	constexpr auto IdcArrow = PlatformShared::ConstexprValue<IDC_ARROW>{};

	namespace FormatMessageFlags
	{
		enum
		{
			FromSystem = FORMAT_MESSAGE_FROM_SYSTEM,
			AllocateBuffer = FORMAT_MESSAGE_ALLOCATE_BUFFER,
			IgnoreInserts = FORMAT_MESSAGE_IGNORE_INSERTS
		};
	}

	namespace WindowStyles
	{
		enum
		{
			OverlappedWindow = WS_OVERLAPPEDWINDOW
		};
	}

	namespace CreateWindowFlags
	{
		enum
		{
			UseDefault = CW_USEDEFAULT
		};
	}

	namespace  Gwlp
	{
		enum
		{
			UserData = GWLP_USERDATA
		};
	}

	namespace Messages
	{
		enum : UINT
		{
			Quit = WM_QUIT,
			NonClientCreate = WM_NCCREATE,
			NonClientDestroy = WM_NCDESTROY,
			Destroy = WM_DESTROY,
			Close = WM_CLOSE,
			Paint = WM_PAINT,
			KeyUp = WM_KEYUP,
			KeyDown = WM_KEYDOWN,
			Command = WM_COMMAND,
			Size = WM_SIZE,
			Sizing = WM_SIZING,
			ExitSizeMove = WM_EXITSIZEMOVE,
			DisplayChange = WM_DISPLAYCHANGE,
			DpiChanged = WM_DPICHANGED,
			Timer = WM_TIMER
		};
	}

	namespace WindowSizeState
	{
		enum : WPARAM
		{
			Restored = SIZE_RESTORED,
			Minimized = SIZE_MINIMIZED,
			Maximized = SIZE_MAXIMIZED
		};
	}

	namespace VirtualKey
	{
		enum : WPARAM
		{
			Escape = VK_ESCAPE,
			Left = VK_LEFT,
			Up = VK_UP,
			Right = VK_RIGHT,
			Down = VK_DOWN,
			Space = VK_SPACE,
		};
	}

	namespace PeekMessageFlags
	{
		enum
		{
			NoRemove = PM_NOREMOVE,
			Remove = PM_REMOVE,
			NoYield = PM_NOYIELD
		};
	}

	namespace ShowWindowFlags
	{
		enum
		{
			Show = SW_SHOW,
			Hide = SW_HIDE,
			ShowDefault = SW_SHOWNORMAL
		};
	}
}