#include "include.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>
#include <string>
#include <cstdio>
#include <iostream>

void creat_and_write(	std::string& content)
{
	std::ostringstream oss;
	oss << "DataBase/File_" << std::time(NULL);
	std::string filename = oss.str();
	int fd = open(filename.c_str(), O_CREAT | O_WRONLY, 0666);
	if (fd < 0)
	{
		std::cerr << "Failed to open file " << filename << "\n";
		return;
	}
	ssize_t written = write(fd, content.c_str(), content.size());
	if (written < 0)
		std::cerr << "Failed to write to file " << filename << "\n";
	close(fd);
	std::cout << "Written " << written << " bytes to " << filename << "\n";
}


