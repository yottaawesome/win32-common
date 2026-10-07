export module Shared:Window;
import std;
import Win32;
import :Win32Raii;
import :Win32Error;

export namespace Win32::UI
{
	struct Dimensions
	{
		std::uint32_t Width = 0;
		std::uint32_t Height = 0;
	};

	class Window
	{
	public:
		template<typename... TArgs>
		using Fn = std::move_only_function<auto(TArgs...) -> void>;

		struct Args
		{
			std::uint32_t Width = 800;
			std::uint32_t Height = 600;
			std::wstring WindowClassName = L"Main Window Class";
			std::wstring Title = L"Main Window";
			bool InitiallyVisible = true;
			Fn<std::uint32_t, std::uint32_t> OnResize
				= [](auto, auto) {};
			Fn<std::uint32_t, std::uint32_t> OnMinimized
				= [](auto, auto) {};
			Fn<std::uint32_t, std::uint32_t> OnMaximized
				= [](auto, auto) {};
			Fn<std::uint32_t, std::uint32_t> OnRestored
				= [](auto, auto) {};
			Fn<std::exception_ptr> OnUnhandledException
				= [](auto) {};
			Fn<Win32::LPARAM, Win32::WPARAM> OnKeyDown
				= [](auto, auto) {};
			Fn<Win32::LPARAM, Win32::WPARAM> OnKeyUp
				= [](auto, auto) {};
		};

		Window(Args args)
			: OnResize(std::move(args.OnResize))
			, OnMinimized(std::move(args.OnMinimized))
			, OnMaximized(std::move(args.OnMaximized))
			, OnRestored(std::move(args.OnRestored))
			, OnUnhandledException(std::move(args.OnUnhandledException))
			, OnKeyDown(std::move(args.OnKeyDown))
			, OnKeyUp(std::move(args.OnKeyUp))
		{
			Initialize(args);
		}

		Fn<std::uint32_t, std::uint32_t> OnResize;
		Fn<std::uint32_t, std::uint32_t> OnMinimized;
		Fn<std::uint32_t, std::uint32_t> OnMaximized;
		Fn<std::uint32_t, std::uint32_t> OnRestored;
		Fn<std::exception_ptr> OnUnhandledException;
		Fn<Win32::LPARAM, Win32::WPARAM> OnKeyDown;
		Fn<Win32::LPARAM, Win32::WPARAM> OnKeyUp;

		auto GetClientWidth(this auto&& self) noexcept -> std::uint32_t
		{
			auto rect = Win32::RECT{};
			Win32::GetClientRect(self.GetHwnd(), &rect);
			return static_cast<std::uint32_t>(rect.right - rect.left);
		}

		auto GetClientHeight(this auto&& self) noexcept -> std::uint32_t
		{
			auto rect = Win32::RECT{};
			Win32::GetClientRect(self.GetHwnd(), &rect);
			return static_cast<std::uint32_t>(rect.bottom - rect.top);
		}

		auto GetClientDimensions(this auto&& self) noexcept -> Dimensions
		{
			auto rect = Win32::RECT{};
			Win32::GetClientRect(self.GetHwnd(), &rect);
			return {
				static_cast<std::uint32_t>(rect.right - rect.left),
				static_cast<std::uint32_t>(rect.bottom - rect.top)
			};
		}

		auto GetDpiForWindow(this auto&& self) noexcept -> std::uint32_t
		{
			return static_cast<std::uint32_t>(Win32::GetDpiForWindow(self.GetHwnd()));
		}

		auto GetHwnd(this auto&& self) noexcept -> Win32::HWND
		{
			return self.m_window.get();
		}

	private:
		void Initialize(const Args& args)
		{
			//
			// Register the window class.
			auto wndClass = Win32::WNDCLASSEXW{
				.cbSize = sizeof(Win32::WNDCLASSEXW),
				.style = 0,
				.lpfnWndProc = &Window::WindowProc,
				.cbClsExtra = 0,
				.cbWndExtra = 0,
				.hInstance = Win32::GetModuleHandleW(nullptr),
				.hIcon = Win32::LoadIconW(nullptr, Win32::IdiApplication),
				.hCursor = Win32::LoadCursorW(nullptr, Win32::IdcArrow),
				.hbrBackground = static_cast<Win32::HBRUSH>(Win32::GetStockObject(Win32::Brushes::White)),
				.lpszMenuName = nullptr,
				.lpszClassName = args.WindowClassName.c_str(),
			};
			if (Win32::RegisterClassExW(&wndClass) == 0)
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to register window class" };

			auto screenWidth = Win32::GetSystemMetrics(Win32::SystemMetrics::ScreenWidth);
			auto initialPosX = (screenWidth - args.Width) / 2;
			auto screenHeight = Win32::GetSystemMetrics(Win32::SystemMetrics::ScreenHeight);
			auto initialPosY = (screenHeight - args.Height) / 2;

			//
			// Create the window.
			auto hwnd = Win32::CreateWindowExW(
				0,
				args.WindowClassName.c_str(),
				args.Title.c_str(),
				Win32::WindowStyles::OverlappedWindow,
				initialPosX,
				initialPosY,
				args.Width,
				args.Height,
				nullptr,
				nullptr,
				wndClass.hInstance,
				this
			);
			if (hwnd == nullptr)
				throw Win32::Error::Win32Error{ Win32::GetLastError(), "Failed to create window" };
			if (args.InitiallyVisible)
				Win32::ShowWindow(hwnd, Win32::ShowWindowFlags::Show);
			else 
				Win32::ShowWindow(hwnd, Win32::ShowWindowFlags::Hide);
		}

		//
		// Static windows procedure. Dispatches messages to the appropriate instance of the Window class.
		static auto WindowProc(
			Win32::HWND hWnd, 
			Win32::UINT uMsg, 
			Win32::WPARAM wParam, 
			Win32::LPARAM lParam
		) -> Win32::LRESULT
		{
			auto pThis = static_cast<Window*>(nullptr);
			if (uMsg == Win32::Messages::NonClientCreate)
			{
				auto* pCreate = reinterpret_cast<Win32::CREATESTRUCT*>(lParam);
				pThis = reinterpret_cast<Window*>(pCreate->lpCreateParams);
				Win32::SetWindowLongPtrW(hWnd, Win32::Gwlp::UserData, reinterpret_cast<Win32::LONG_PTR>(pThis));

				// Adopt ownership once, during creation
				pThis->m_window = HwndUniquePtr{ hWnd };
			}
			else
			{
				pThis = reinterpret_cast<Window*>(Win32::GetWindowLongPtrW(hWnd, Win32::Gwlp::UserData));
				// Detach before the OS destroys the HWND to avoid double-destroy later.
				// More information here https://learn.microsoft.com/en-us/cpp/mfc/tn017-destroying-window-objects?view=msvc-170
				// under the "Auto cleanup with CWnd::PostNcDestroy" header.
				if (pThis and uMsg == Win32::Messages::NonClientDestroy)
				{
					Win32::SetWindowLongPtrW(hWnd, Win32::Gwlp::UserData, 0);
					pThis->m_window.release();
				}
			}

			if (uMsg == Win32::Messages::Destroy)
				return (Win32::PostQuitMessage(0), 0);
			else if (pThis)
				return pThis->HandleMessage(uMsg, wParam, lParam);
			else
				return Win32::DefWindowProcW(hWnd, uMsg, wParam, lParam);
		}

		auto HandleMessage(Win32::UINT uMsg, Win32::WPARAM wParam, Win32::LPARAM lParam) -> Win32::LRESULT
		try
		{
			switch (uMsg)
			{
				case Win32::Messages::Destroy:
				{
					Win32::PostQuitMessage(0);
					return 0;
				}

				// https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-size
				case Win32::Messages::Size:
				{
					auto width = std::uint32_t{ Win32::LoWord(lParam) };
					auto height = std::uint32_t{ Win32::HiWord(lParam) };

					if (wParam == Win32::WindowSizeState::Minimized)
					{
						OnMinimized(width, height);
					}
					else if (wParam == Win32::WindowSizeState::Maximized)
					{
						OnMaximized(width, height);
					}
					else if (wParam == Win32::WindowSizeState::Restored)
					{
						OnRestored(width, height);
					}

					return 0;
				}

				case Win32::Messages::ExitSizeMove:
				{
					OnResize(GetClientWidth(), GetClientHeight());
					return 0;
				}

				case Win32::Messages::DpiChanged:
				{
					auto newDpiX = Win32::LoWord(wParam);
					auto newDpiY = Win32::HiWord(wParam);
					auto suggestedRect = reinterpret_cast<Win32::RECT*>(lParam);
					auto succeeded = Win32::SetWindowPos(
						m_window.get(),
						nullptr,
						suggestedRect->left,
						suggestedRect->top,
						suggestedRect->right - suggestedRect->left,
						suggestedRect->bottom - suggestedRect->top,
						Win32::SetWindowPosFlags::NoZOrder | Win32::SetWindowPosFlags::NoActivate
					);
					if (not succeeded)
						throw Error::Win32Error{ Win32::GetLastError(), "Failed to set window position after DPI change" };
					return 0;
				}

				case Win32::Messages::KeyDown:
				{
					auto keyCode = static_cast<std::uint32_t>(wParam);
					OnKeyDown(lParam, wParam);
					return 0;
				}

				case Win32::Messages::KeyUp:
				{
					auto keyCode = static_cast<std::uint32_t>(wParam);
					OnKeyUp(lParam, wParam);
					return 0;
				}

				default:
					return Win32::DefWindowProcW(m_window.get(), uMsg, wParam, lParam);
			}
		}
		catch (...)
		{
			OnUnhandledException(std::current_exception());
			Win32::PostQuitMessage(1);
			return 0;
		}

	private:
		Win32::HwndUniquePtr m_window;
	};
}