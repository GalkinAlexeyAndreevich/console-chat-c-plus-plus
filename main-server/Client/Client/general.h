#pragma once
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <windows.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <experimental/filesystem>


using namespace std;

namespace fs = std::experimental::filesystem;

