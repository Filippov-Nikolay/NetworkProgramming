
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 4096

#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT "8888"

SOCKET client_socket;

COORD point{ 0, 0 };//  оордината точки
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); // получаем адрес собственного окна

struct InfoUser {
    char name[DEFAULT_BUFLEN];
    int color;
};
InfoUser infoUser;
bool fFisr = true;
int position = 4;

DWORD WINAPI Sender(void* param)
{
    while (true) {
        // cout << "Please insert your query for server: ";
        char query[DEFAULT_BUFLEN] = "";
        char temp[DEFAULT_BUFLEN] = "";

        if (fFisr) {
            strcat_s(query, DEFAULT_BUFLEN, "\tNew user: ");
            strcat_s(query, DEFAULT_BUFLEN, infoUser.name);
            strcat_s(query, DEFAULT_BUFLEN, "\n");

            send(client_socket, query, strlen(query), 0);

            fFisr = false;
            cout << "Enter msg: ";
        }
        else {
            cin.getline(temp, DEFAULT_BUFLEN);
            strcat_s(temp, DEFAULT_BUFLEN, "\n");

            strcat_s(query, DEFAULT_BUFLEN, "New msg (");
            strcat_s(query, DEFAULT_BUFLEN, infoUser.name);
            strcat_s(query, DEFAULT_BUFLEN, "): ");

            strcat_s(query, DEFAULT_BUFLEN, temp);

            send(client_socket, query, strlen(query), 0);
        }

        point.Y += position++;     // строки
        // point.X += 12;  // столбцы

        SetConsoleCursorPosition(h, point); // установка курсора в указанную позицию!
        SetConsoleTextAttribute(h, infoUser.color);
    }
}

DWORD WINAPI Receiver(void* param)
{
    while (true) {
        char response[DEFAULT_BUFLEN];
        int result = recv(client_socket, response, DEFAULT_BUFLEN, 0);
        response[result] = '\0';

        // cout << "...\nYou have new response from server: " << response << "\n";
        cout << response << "\n";
        // cout << "Please insert your query for server: ";
        cout << "Enter msg: ";

        position+=5;
    }
}

BOOL ExitHandler(DWORD whatHappening)
{
    switch (whatHappening)
    {
    case CTRL_C_EVENT: // closing console by ctrl + c
    case CTRL_BREAK_EVENT: // ctrl + break
    case CTRL_CLOSE_EVENT: // closing the console window by X button
      return(TRUE);
        break;
    default:
        return FALSE;
    }
}

int main()
{
    // обработчик закрыти€ окна консоли
    //SetConsoleCtrlHandler((PHANDLER_ROUTINE)ExitHandler, true);

    system("title Client");

    // initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // разрешить адрес сервера и порт
    addrinfo* result = nullptr;
    iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 2;
    }

    addrinfo* ptr = nullptr;
    // пытатьс€ подключитьс€ к адресу, пока не удастс€
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // создать сокет на стороне клиента дл€ подключени€ к серверу
        client_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (client_socket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 3;
        }

        // connect to server
        iResult = connect(client_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (client_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 5;
    }

    cout << "Enter your name: ";
    cin.getline(infoUser.name, DEFAULT_BUFLEN);

    do {
        cout << "Enter your color(number 1 - 15): ";
        cin >> infoUser.color;

        if (infoUser.color >= 16) {
            cout << "Invalid range\n";
        }
    } while (infoUser.color >= 16);
    

    cin.ignore();

    CreateThread(0, 0, Receiver, 0, 0, 0);
    CreateThread(0, 0, Sender, 0, 0, 0);

    Sleep(INFINITE);
}