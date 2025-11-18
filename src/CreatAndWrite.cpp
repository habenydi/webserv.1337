#include "include.hpp"

void    creat_and_write(std::string& content, std::string& filename)
{
	std::remove(filename.c_str());
	int fd = open(filename.c_str(), O_CREAT | O_WRONLY);
	write (fd, content.c_str(), content.size());
	close (fd);
}