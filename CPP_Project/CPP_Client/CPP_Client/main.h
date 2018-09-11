#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef CLIENT_MAIN_H

#define CLIENT_MAIN_H

#include <WinSock2.h>
#include<stdio.h>
#include <iostream>
using std::cout;
using std::endl;

#include <thread>
#include <chrono>

#include <string>
#include <stdexcept>

#define SERVER_PORT 5001
#define BUF_SIZE 1024
#define QUEUE_SIZE 10
#define IPAddress "192.168.1.61"

#endif // !CLIENT_MAIN_H