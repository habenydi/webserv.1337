#include "include.hpp"

void    creat_and_write(std::string& content, std::string& filename)
{
	std::remove(content.c_str());
	int fd = open(content.c_str(), O_CREAT | O_WRONLY);
	write (fd, filename.c_str(), filename.size());
	close (fd);
}