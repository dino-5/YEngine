#include "Filepath.h"
#include "Logger.h"

Filepath::Filepath(std::string name):m_path(name)
{
	if (std::filesystem::exists(m_path))
		Logger::PrintError("{} is not exist", name);
}
