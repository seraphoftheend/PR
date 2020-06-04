#pragma once
#pragma comment (lib, "Urlmon.lib")
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <regex>
#include <tchar.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <fstream>
#include <mutex>
#include <chrono>


using namespace std;

const int default_buflen = 512;

int initializeWinSock();
SOCKET createConnection(string, string);
int sendRequest(SOCKET, string);
int receiveHead(SOCKET,int);
string _receiveResponse(SOCKET,int);
string receiveResponse(SOCKET);
vector <string> findImages(string s);