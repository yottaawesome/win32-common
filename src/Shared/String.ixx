export module Shared:String;
import std;

export namespace String
{
	[[nodiscard]]
	auto TokeniseString(const std::string& stringToTokenise, const std::string& delimiter) -> std::vector<std::string>
	{
		auto position = size_t{};
		// If we don't find it at all, add the whole string
		if (stringToTokenise.find(delimiter, position) == std::string::npos)
			return { stringToTokenise };

		auto results = std::vector<std::string>{};
		auto intermediateString = std::string{ stringToTokenise };
		while ((position = intermediateString.find(delimiter)) != std::string::npos)
		{
			// split and add to the results
			auto split = std::string{ intermediateString.substr(0, position) };
			results.push_back(split);

			// move up our position
			position += delimiter.length();
			intermediateString = intermediateString.substr(position);

			// On the last iteration, enter the remainder
			if (intermediateString.find(delimiter) == std::string::npos)
				results.push_back(intermediateString);
		}

		return results;
	}
}
