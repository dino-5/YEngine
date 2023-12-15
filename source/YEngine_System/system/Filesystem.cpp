#include "Filesystem.h"
#include "Logger.h"
#include "IOManager.h"

#include <fstream>

Filepath::Filepath(std::string name):m_path(name)
{
	if (!filesystem::exists(m_path))
	{
		Logger::PrintError("{} is not exist", name);
		throw std::runtime_error(fmt::format("{} is not exist", name));
	}
}


File::File(std::string fileName) : m_path(fileName)
{
}

std::string File::readFile()
{
	std::ifstream file(m_path.getPath());
	std::ostringstream s_str;
	s_str << file.rdbuf();
	return s_str.str();
}
