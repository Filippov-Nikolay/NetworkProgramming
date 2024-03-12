#include <iostream>
#include <string>
#include "WinSock2.h" //здесь находятся объявления, необходимые
//для Winsock 2 API.
#include <ws2tcpip.h> //содержит функции для работы с адресами
//напр. inet_pton
#pragma comment(lib, "Ws2_32.lib") //линкуем библиотеку Windows Sockets

using namespace std;

void main()
{
	const int MAXSTRLEN = 255;
	WSADATA wsaData;//структура для хранения информацию
					//о инициализации сокетов
	SOCKET _socket; //дескриптор слушающего сокета
	SOCKET acceptSocket;//дескриптор сокета, который связан с клиентом 
	sockaddr_in addr; //локальный адрес и порт
	//инициализация сокетов
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//создаем потоковый сокет, протокол TCP
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//Семейство адресов IPv4
	addr.sin_family = AF_INET;
	/*Преобразуем адрес в правильную
	структуру хранения адресов, результат сохраняем в поле sin_addr */
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	//Указываем порт. 
	//Функиця htons выполняет преобразование числа в
	//сетевой порядок байт, т.к. стандартным сетевым 
	//является порядок от старшего к младшему
	addr.sin_port = htons(20000);
	//биндим сокет к указанному адресу и порту
	bind(_socket, (SOCKADDR*)&addr, sizeof(addr));
	//открываем сокет в режиме прослушивания входящих подключений
	listen(_socket, 1);
	cout << "Server is started\n";
	//ожидаем подключения клиента.
	//в случае успешного подключения - получаем новый сокет
	//через который будет происходить обмен сообщениями
	//с клиентом.
	//Функция является блокирующей.
	acceptSocket = accept(_socket, NULL, NULL);
	
	char buf[MAXSTRLEN];
	char str[MAXSTRLEN];

	cout << "\nEnter text:\n";
	cin.getline(str, MAXSTRLEN);

	//Отправляем сообщение клиенту
	send(acceptSocket, str, strlen(str), 0);

	//Получаем сообщение от клиента
	//Функция является блокирующей
	int i = recv(acceptSocket, buf, MAXSTRLEN, 0);
	buf[i] = '\0';

	//Закрываем сокеты
	closesocket(acceptSocket);
	closesocket(_socket);
	//Освобождаем ресурсы
	WSACleanup();
	system("pause");
}