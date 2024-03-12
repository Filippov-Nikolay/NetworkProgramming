#include <iostream>
#include <string>
#include "WinSock2.h" //здесь находятся объявления, необходимые
 //для Winsock 2 API.
#include <ws2tcpip.h> //содержит функции для работы с адресами
 //напр. inet_pton
#pragma comment(lib, "Ws2_32.lib") //линкуем библиотеку Windows Sockets

using namespace std;


int main()
{
	const int MAXSTRLEN = 255;
	WSADATA wsaData;//структура для хранения информацию
					//о инициализации сокетов
	SOCKET _socket; //дескриптор сокета
	sockaddr_in addr; //локальный адрес и порт
	//инициализация сокетов
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//создаем потоковый сокет, протокол TCP
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//Семейство адресов IPv4
	addr.sin_family = AF_INET;
	/*Преобразуем адрес в правильную
	структуру хранения адресов, результат сохраняем в поле sin_addr */
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	//Указываем порт. 
	//Функция htons выполняет преобразование числа в
	//сетевой порядок байт, т.к. стандартным сетевым 
	//является порядок от старшего к младшему
	addr.sin_port = htons(20000);
	//выполняем подключение к серверу, указывая
	//дескриптор сокета, адрес и размер структуры
	connect(_socket, (SOCKADDR*)&addr, sizeof(addr));
	
	char buf[MAXSTRLEN];
	const char * text = "Hello world!";

	cout << "\nPress Enter to send 'Hello world!' to server\n";
	//Ожидаем подтверждения отправки сообщения пользователем
	//(нажатие клавиши)
	cin.get();

	//Отправляем сообщение на указанный сокет
	send(_socket, text, strlen(text), 0);

	//Принимаем сообщение от сервера
	//Функция является блокирующей
	int i = recv(_socket, buf, MAXSTRLEN, 0);
	buf[i] = '\0';

	cout << buf << endl;

	//Закрываем сокет
	closesocket(_socket);
	//Освобождаем ресурсы
	WSACleanup();
	system("pause");
}

