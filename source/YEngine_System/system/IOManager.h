#pragma once
#include "third_party/fmt/include/fmt/printf.h"
#include "third_party/fmt/include/fmt/core.h"

#include <string>
#include <format>

namespace io
{
	template<typename ...Args>
	void print(fmt::format_string<Args...> string, Args... args)
	{
		fmt::print(fmt::runtime(string), std::forward<Args...>(args)...);
	}

	void print(std::string s);
};
