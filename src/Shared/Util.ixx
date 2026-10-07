export module Shared:Util;
import std;

export namespace Util
{
	template<typename...T>
	struct Overload : T...
	{
		using T::operator()...;
	};

	template<typename...T>
	struct Variant
	{
		using TVariant = std::variant<T...>;
		TVariant Value;

		constexpr auto operator()(auto&&...visitor) -> decltype(auto)
		{
			return std::visit(Overload{ std::forward<decltype(visitor)>(visitor)... }, Value);
		}

		constexpr auto operator=(auto&& newValue) -> Variant&
		{
			Value = std::forward<decltype(newValue)>(newValue);
			return *this;
		}
	};
}
