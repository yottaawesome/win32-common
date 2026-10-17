export module PlatformShared;
import std;

// Wraps a constant value that otherwise can't be constexpr in a callable object that can be used in constexpr contexts.
export namespace PlatformShared
{
	template<auto VValue>
	struct ConstexprValue
	{
		static constexpr auto operator()() noexcept -> decltype(VValue)
		{
			return VValue;
		}

		constexpr operator decltype(VValue)() const noexcept
		{
			return VValue;
		}
	};

	template<std::invocable auto VValue>
	struct InvocableValue
	{
		static constexpr auto operator()() noexcept -> std::invoke_result_t<decltype(VValue)>
		{
			return std::invoke(VValue);
		}

		constexpr operator std::invoke_result_t<decltype(VValue)>() const noexcept
		{
			return std::invoke(VValue);
		}
	};
}
