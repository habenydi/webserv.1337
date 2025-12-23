#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <exception>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cstdlib>
#include <csignal>
#include <ostream>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctime>
#include <sys/stat.h>
#include <cstdio>
#include <csignal>
#include <utility>

#define MAX_REQUEST_SIZE 5362
# define MAX_TIME_LOGIN 1800
