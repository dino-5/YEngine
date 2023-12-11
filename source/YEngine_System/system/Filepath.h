#pragma once
#include <filesystem>

class Filepath
{
public:
	Filepath(std::string name);
private:
	std::filesystem::path m_path;
};

