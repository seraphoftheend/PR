#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include "Functions.h"
#include <iostream>


int initializeWinSock() {
	WSADATA wsaData;
	int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    return 0;
}


SOCKET createConnection(string hostname, string port) {
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
                   * ptr = NULL,
                     hints;
    int iResult;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        return ConnectSocket;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            return ConnectSocket;
        }

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);
    return ConnectSocket;
}


int sendRequest(SOCKET ConnectSocket, string sendbuf) {
    int iResult = 0;
  
    iResult = send(ConnectSocket, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Bytes Sent: %ld\n", iResult); 
    return 0;
}


vector <string> findImages(string s) {
    smatch m;
    regex e(R"(("\S*\.png)|("\S*\.jpg)|("\S*\.gif))");
    static vector <string> v;
    string tmp = "";

    while (regex_search(s, m, e)) {
        tmp = m[0];
        tmp.erase(0, 1);
        v.push_back(tmp);
        s = m.suffix().str();
    }

    return v;
}