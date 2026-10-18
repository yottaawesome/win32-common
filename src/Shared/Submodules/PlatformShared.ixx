export module PlatformShared;
import std;

// Wraps a constant value that otherwise can't be constexpr in a callable object that can be used in constexpr contexts.
export namespace PlatformShared
{
	template<auto VValue>
	struct ConstexprValue
	{
		using ValueType = decltype(
			[] -> auto
			{
				if constexpr (std::invocable<decltype(VValue)>)
					return std::invoke(VValue);
				else
					return VValue;
			}());

		static constexpr auto operator()() noexcept -> ValueType
		{
			if constexpr (std::invocable<decltype(VValue)>)
				return std::invoke(VValue);
			else
				return VValue;
		}

		constexpr operator ValueType() const noexcept
		{
			return operator()();
		}
	};
}
