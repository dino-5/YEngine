#pragma once
#include <format>
#include <string>
#include <vector>
#include <iostream>

struct LoggerState
{
#define LoggerVariable(varName) bool varName = true;\
void switch##varName(bool val) { varName = val;}
	LoggerVariable(infoEnabled)
	LoggerVariable(errorsEnabled)
};
// ANSI escape code for red text
constexpr const char* redColor = "\033[1;31m";

// ANSI escape code for resetting text color
constexpr const char* resetColor = "\033[0m";

class Logger
{
public:
	template<typename... Args>
	static void PrintInfo(std::format_string<Args...> fmt, Args&&... args)
	{
		s_info.push_back(std::format(fmt, std::forward<Args>(args)...));
	}
	template<typename... Args>
	static void PrintError(std::format_string<Args...> fmt, Args&&... args)
	{
		s_errors.push_back(std::format(fmt, std::forward<Args>(args)...));
	}
	static void Update()
	{
		if (s_state.infoEnabled)
			for (auto& info : s_info)
				std::cout << "[INFO] " << info;
		if (s_state.errorsEnabled)
		{
			std::cout << redColor;
			for (auto& info : s_errors)
				std::cout << "[ERROR] " << info;
			std::cout << resetColor;
		}
		s_info.clear();
		s_errors.clear();
	}

	static inline LoggerState s_state;
private:
	static inline std::vector<std::string> s_info;
	static inline std::vector<std::string> s_errors;
};

