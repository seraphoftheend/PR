#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <tchar.h>
#include "Functions.h"

using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

vector <string> imagesSRC;
int absnum = 0;
char filename[1000] = {  };
bool fail = 0;
int headSize[200];
int imgSize[200];

int main()
{
    SOCKET socketMainTr;
    string host = "unite.md";
    string port = "80";
    string sendBuffer = "GET / HTTP/1.1\r\nHost: unite.md\r\n\r\n";
    string website_HTML = "";
    
    mutex mtx;
    int imageNumber = 0;

    if (initializeWinSock()) {
        return 1;
    }

    socketMainTr = createConnection(host, port);
    if (socketMainTr == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    if (sendRequest(socketMainTr, sendBuffer)) {
        WSACleanup();
        return 1;
    }

    website_HTML = receiveResponse(socketMainTr);

    closesocket(socketMainTr);

    imagesSRC = findImages(website_HTML);

    for (uint16_t i = 0; i < imagesSRC.size(); i++) {
        cout << i << " " << imagesSRC[i] << endl;
    }
   
    thread tr1([&]() {

        string sendBufferTr = "";
        string sendBufferTrH = "";
        int headResponse;
        string number = "";
        int n = 0;
        SOCKET socketTr1;
        int posgl = 0;
        while (imageNumber < imagesSRC.size())
        {
            socketTr1 = createConnection(host, port);
            mtx.lock();
            if (imageNumber < imagesSRC.size()) {
                sendBufferTrH = "HEAD " + imagesSRC[imageNumber] + " HTTP/1.1\r\nHost: unite.md\r\n\r\n";
                sendBufferTr = "GET " + imagesSRC[imageNumber] + " HTTP/1.1\r\nHost: unite.md\r\n\r\n";
                posgl = imageNumber;
                imageNumber++;
            }          

            mtx.unlock();
            string image = "";
            if (sendRequest(socketTr1, sendBufferTrH)) {
                WSACleanup();
                return 1;
            }
            
            headResponse = receiveHead(socketTr1,posgl);          
            closesocket(socketTr1);          
                      
            socketTr1 = createConnection(host, port);
            if (sendRequest(socketTr1, sendBufferTr)) {
                WSACleanup();
                return 1;
            }
            
            image = _receiveResponse(socketTr1, headResponse);
         
            closesocket(socketTr1);           
        }
        });

    thread tr2([&]() {

        string sendBufferTr = "";
        string sendBufferTrH = "";
        int headResponse;
        string number = "";
        int n = 0;
        SOCKET socketTr2;
        int posgl = 0;
        while (imageNumber < imagesSRC.size())
        {
            socketTr2 = createConnection(host, port);
            mtx.lock();
            if (imageNumber < imagesSRC.size()) {
                sendBufferTrH = "HEAD " + imagesSRC[imageNumber] + " HTTP/1.1\r\nHost: unite.md\r\n\r\n";
                sendBufferTr = "GET " + imagesSRC[imageNumber] + " HTTP/1.1\r\nHost: unite.md\r\n\r\n";
                posgl = imageNumber;
                imageNumber++;
            }
            mtx.unlock();
            string image = "";
            if (sendRequest(socketTr2, sendBufferTrH)) {
                WSACleanup();
                return 1;
            }
           
            headResponse = receiveHead(socketTr2,posgl);
            closesocket(socketTr2);        
          
            socketTr2 = createConnection(host, port);
            if (sendRequest(socketTr2, sendBufferTr)) {
                WSACleanup();
                return 1;
            }
           
            image = _receiveResponse(socketTr2, headResponse);
          
            closesocket(socketTr2);       
        }
        });


    thread tr3([&]() {

        string sendBufferTr = "";
        string sendBufferTrH = "";
        int headResponse;
        string number = "";
        int n = 0;
        SOCKET socketTr3;
        int posgl = 0;
        while (imageNumber < imagesSRC.size())
        {
            socketTr3 = createConnection(host, port);
            mtx.lock();
            if (imageNumber < imagesSRC.size()) {
                sendBufferTrH = "HEAD " + imagesSRC[imageNumber] + " HTTP/1.1\r\nHost: unite.md\r\n\r\n";
                sendBufferTr = "GET " + imagesSRC[imageNumber] + " HTTP/1.1\r\nHost: unite.md\r\n\r\n";
                posgl = imageNumber;
                imageNumber++;
            }
            mtx.unlock();
            string image = "";
            if (sendRequest(socketTr3, sendBufferTrH)) {
                WSACleanup();
                return 1;
            }
           
            headResponse = receiveHead(socketTr3,posgl);
            closesocket(socketTr3);

            socketTr3 = createConnection(host, port);
            if (sendRequest(socketTr3, sendBufferTr)) {
                WSACleanup();
                return 1;
            }
           
            image = _receiveResponse(socketTr3, headResponse);
            closesocket(socketTr3);
        }
        });

    tr1.join();
    tr2.join();
    tr3.join();
    WSACleanup();
    return 0;
}



string _receiveResponse(SOCKET ConnectSocket, int pos) {

    bool stop = 0;
    int iResult = 0;
    char* recvbuf = new char[headSize[pos-1]];

    iResult = recv(ConnectSocket, recvbuf, headSize[pos-1], 0);

    int i = 0;
    char* img = new char[imgSize[pos-1]];
    char* help = new char[1246];

    for (i = 0; i < imgSize[pos-1] / 1246; i++)
    {
        iResult = recv(ConnectSocket, help, 1246, 0);

        for (int j = 0; j < 1246; j++)
            img[j + i * 1246] = help[j];
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    }
    iResult = recv(ConnectSocket, help, imgSize[pos-1] - i * 1246, 0);

    for (int j = 0; j < imgSize[pos-1] - i * 1246; j++)
        img[j + i * 1246] = help[j];
    if (iResult > 0)
        printf("Bytes received: %d\n", iResult);
    else if (iResult == 0)
        printf("Connection closed\n");
    else
        printf("recv failed: %d\n", WSAGetLastError());
    FILE* bin;
   
    string filename = imagesSRC[pos - 1];
    for (int m = 0; m < filename.size(); m++)
        if (filename[m] == '/')
            filename[m] = 'I';

    bin = fopen(filename.c_str(), "wb");
    fwrite(img, 1U, imgSize[pos - 1], bin);
    
    fclose(bin);
    free(img);
    free(help);
    return " ";
}

int receiveHead(SOCKET ConnectSocket, int posGL) {

    int iResult = 0;
    char recvbuf[513];
    int recvbuflen = 512;
    string data = "";
    string number = "";
    int pos = 0;

    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        data += recvbuf;
        
        if (iResult > 0) {          
                headSize[posGL] = iResult; posGL++; pos = posGL;
                
                if (iResult >= 200)
                {
                    size_t end = data.find_first_of("1234567890", 170);
                    size_t start = data.find_first_of("1234567890", end + 1);
                    size_t start1 = data.find_first_of("1234567890", start + 1);
                    while (start - start1 == -1)
                    {
                        start = start1;
                        start1 = data.find_first_of("1234567890", start + 1);
                    }
                    for (int i = end; i <= start; i++)
                    {
                        number += data[i];
                    }

                    imgSize[pos-1] = atoi(number.c_str());
                }
            
            printf("Bytes received: %d\n", iResult);
        }
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
    } while (iResult > 0);

    return pos;
}

string receiveResponse(SOCKET ConnectSocket) {

    int iResult = 0;
    char recvbuf[513];
    int recvbuflen = 512;
    string data = "";
    string number = "";

    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        data += recvbuf;

        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
        }
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
    } while (iResult > 0);

    return data;
}