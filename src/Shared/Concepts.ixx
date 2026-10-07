export module Shared:Concepts;
import std;

export namespace Concepts
{
	template<typename T, typename...R>
	concept OneOf = (std::same_as<T, R> or ...);

	template<class T>
	struct IsDuration : std::false_type {};

	template<class Rep, class Period>
	struct IsDuration<std::chrono::duration<Rep, Period>> : std::true_type {};

	template<typename T>
	constexpr auto IsDurationV = IsDuration<T>::value;

	template<typename T>
	concept Duration = IsDurationV<T>;
}