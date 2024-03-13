#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include <iostream>
using namespace std;

int main()
{
    WSADATA wsadata;

    int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (res != NO_ERROR)
    {
        cout << "WSAStartup failked with error " << res << endl;
        return 1;
    }

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        cout << "socket failed with error " << WSAGetLastError() << endl;
        return 2;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;    
    addr.sin_port = htons(23000);   
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    if (bind(udpSocket, (SOCKADDR*)&addr, sizeof(addr)) != NO_ERROR)
    {
        cout << "bind failed with error " << WSAGetLastError() << endl;
        return 3;
    }

    const size_t receiveBufSize = 1024;
    char receiveBuf[receiveBufSize];
    char color[4];
    bool f = false;
    int count = 0;

    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    cout << "Receiving data...." << endl;
    int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&senderAddr, &senderAddrSize);

    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "recvfrom failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    receiveBuf[bytesReceived] = '\0';
    cout << "Received from " << senderAddr.sin_addr.s_host << endl;
    cout << "Data: " << receiveBuf << endl;

    for (int i = 0, j = 0; i < strlen(receiveBuf); i++) {
        if (f)
            color[j++] = receiveBuf[i];

        if (receiveBuf[i] == '.')
            f = true;

        if (j == 2) {
            color[j] = '\0';
            break;
        }
    }

    char str[255] = "";
    strcat_s(str, sizeof(str), "color ");
    strcat_s(str, sizeof(str), color);

    cout << "color: " << color << endl;
    f = false;

    COORD point{ 0, 0 };// Координата точки;
    point.X += 0;
    point.Y += 3;

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); // получаем адрес собственного окна

    SetConsoleCursorPosition(h, point); // установка курсора в указанную позицию!
    SetConsoleTextAttribute(h, 12);

    for (int i = 0, j = 0; i < strlen(receiveBuf); i++) {
        if (receiveBuf[i] == '.')
            break;

        cout << receiveBuf[i];
    }

    SetConsoleTextAttribute(h, 7); // стандарный цвет!

    cout << endl;
    char buff[receiveBufSize];
    for (int i = 0, j = 0; i < strlen(receiveBuf); i++) {
        if (receiveBuf[i] == '.')
            count++;

        if (f)
            buff[j++] = receiveBuf[i];

        if (count == 2)
            f = true;

        if (i == strlen(receiveBuf) - 1)
            buff[j] = '\0';
    }

    cout << buff << endl;

    const size_t sendBufSize = 1024;
    char sendBuf[sendBufSize] = "Hello from UDPExample";

    int sendResult = sendto(udpSocket, sendBuf, strlen(sendBuf), 0, (SOCKADDR*)&senderAddr, senderAddrSize);
    if (sendResult == SOCKET_ERROR) {
        cout << "sendto failed with error " << WSAGetLastError() << endl;
        return 4;
    }


    closesocket(udpSocket);
    WSACleanup();
}

