#pragma once
#include <filesystem>

namespace filesystem = std::filesystem;
using path = std::filesystem::path;

class Filepath
{
public:
	Filepath(std::string name);
	path getPath() { return m_path; }
private:
	path m_path;
};

class File
{
public:
	File(std::string fileName);
	std::string readFile();
private:
	Filepath m_path;
};
