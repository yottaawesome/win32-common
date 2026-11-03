export module Shared:Util.TypeSequence;
import std;

export namespace Util
{
	template<typename...T>
	struct TypeSequence
	{
		template<typename TTypeToCheck>
		constexpr auto Contains() const noexcept -> bool
		{
			return (std::same_as<T, TTypeToCheck> or ...);
		}

		constexpr void ForEach(auto&& visitor) const noexcept
		{
			(visitor.template operator()<T>(), ...);
		}

		constexpr void ForPredicate(auto&& predicate, auto&& fn) const noexcept
		{
			((predicate.template operator()<T>() 
				? std::forward<decltype(fn)>(fn)(T{}) 
				: false)
			or ...);
		}
	};
}