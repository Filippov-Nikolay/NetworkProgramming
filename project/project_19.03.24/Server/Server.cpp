#include <winsock2.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

#define MAX_CLIENTS 30
#define DEFAULT_BUFLEN 4096

// Winsock library
#pragma comment(lib, "ws2_32.lib") 

// Отключаем предупреждение _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4996) 

// Количество пользователей
int numberOfUsers = -1;

// Количество выполненных заказов
int numberOfCompletedOrders = 0;

// Кол-во заказов
int numberOfOrders = 1;

// Menu
int quantity = 3;
int index = 0;

string* item = nullptr;
int* productTime = nullptr;
int* price = nullptr;

SOCKET server_socket;
vector<string> orders;

struct ClientInfo {
	SOCKET socket;
};

string* getWordArray(string text, int& size);

int main() {
	setlocale(0, "ru");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	bool isNext = true;
	const char orderProcessingMsg[DEFAULT_BUFLEN] = "Ваш заказ в обработке";
	const char queueNotificationMsg[DEFAULT_BUFLEN] = "Ваш заказ принят, ожидайте своей очереди";

	item = new string[quantity];
	productTime = new int[quantity];
	price = new int[quantity];

    system("title Server");

    puts("Start server... DONE.");

	// Открытие json файла
	string response, line;
	ifstream in("menu.json", ios::app);

	while (getline(in, line))
		response += line + "\n";

	// cout << response << endl;

	const int BUFFERSIZE = 255;
	char tempKey[BUFFERSIZE];
	bool isWrite = false;

	for (int i = 0, j = 0; i < response.length(); i++) {
		if (response[i] == '"')
			isWrite = true;

		if (isWrite) {
			if (response[i + 1] == '"') {
				tempKey[j] = '\0';

				// cout << tempKey << " ";

				if (strcmp(tempKey, "item") == 0 || strcmp(tempKey, "time") == 0 || strcmp(tempKey, "price") == 0) {
					char tempValue[BUFFERSIZE];

					int a = 4;

					if (strcmp(tempKey, "item") == 0)
						a = 5;

					for (int j = i + a, k = 0; j < response.length(); j++) {
						if (response[j] != '"' && response[j] != ',' && response[j] != '}') {
							tempValue[k++] = response[j];
						}
						else {
							tempValue[k] = '\0';

							// cout << tempValue;

							break;
						}
					}

					if (strcmp(tempKey, "item") == 0) {
						// cout << "Item: " << tempValue << endl;
						item[index] = tempValue;
					}
					else if (strcmp(tempKey, "time") == 0) {
						// cout << "Time: " << tempValue << endl;
						productTime[index] = atoi(tempValue);
					}
					else if (strcmp(tempKey, "price") == 0) {
						// cout << "Price: " << tempValue << endl;
						price[index++] = atoi(tempValue);
					}
				}

				// cout << endl;

				j = 0;
				i++;

				isWrite = false;
			}
			else
				tempKey[j++] = response[i + 1];
		}
	}

	for (int i = 0; i < quantity; i++) {
		cout << "Item: " << item[i] << endl;
		cout << "Time: " << productTime[i] << endl;
		cout << "Price: " << price[i] << endl << endl;
	}


	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 1;
	}

	// Создаём сокет
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket: %d", WSAGetLastError());
		return 2;
	}
	puts("Create socket... DONE.");

	// Подготовка структуры
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code: %d", WSAGetLastError());
		return 3;
	}
	puts("Bind socket... DONE.");

	// Слушать входящие соединения
	listen(server_socket, MAX_CLIENTS);

	// Принять и входящее соединение
	puts("Server is waiting for incoming connections...\nPlease, start one or more client-side app.\n");

	// размер нашего приемного буфера, это длина строки
	// набор дескрипторов сокетов
	// fd means "file descriptors"
	fd_set readfds;
	SOCKET client_socket[MAX_CLIENTS] = {};

	while (true) {
		// Очистить сокет fdset
		FD_ZERO(&readfds);

		// Добавить главный сокет в fdset
		FD_SET(server_socket, &readfds);

		// Добавить дочерние сокеты в fdset
		for (int i = 0; i < MAX_CLIENTS; i++) {
			SOCKET s = client_socket[i];
			if (s > 0) {
				FD_SET(s, &readfds);
			}
		}

		// Дождаться активности на любом из сокетов, тайм-аут равен NULL, поэтому ждите бесконечно
		if (select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) {
			printf("select function call failed with error code : %d", WSAGetLastError());
			return 4;
		}

		// Если что-то произошло на мастер-сокете, то это входящее соединение
		SOCKET new_socket; // Новый клиентский сокет
		sockaddr_in address;
		int addrlen = sizeof(sockaddr_in);
		
		if (FD_ISSET(server_socket, &readfds)) {
			if ((new_socket = accept(server_socket, (sockaddr*)&address, &addrlen)) < 0) {
				perror("accept function error");
				return 5;
			}

			// Информировать серверную сторону о номере сокета - используется в командах отправки и получения
			printf("New connection, socket fd is %d, ip is: %s, port: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
			numberOfUsers++;

			// Добавить новый сокет в массив сокетов
			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (client_socket[i] == 0) {
					client_socket[i] = new_socket;
					printf("Adding to list of sockets at index %d\n", i);
					break;
				}
			}
		}


		 
		for (int i = 0; i < MAX_CLIENTS; i++) {
			SOCKET s = client_socket[i];
			// Если клиент присутствует в сокетах чтения
			if (FD_ISSET(s, &readfds)) {
				// Проверьте, было ли оно на закрытие, а также прочитайте входящее сообщение
				// recv не помещает нулевой терминатор в конец строки (в то время как printf %s предполагает, что он есть)
				char client_message[DEFAULT_BUFLEN] = "";
				int client_message_length = recv(s, client_message, DEFAULT_BUFLEN, 0);

				// lower case
				for (int i = 0; i < client_message_length; i++) {
					client_message[i] = tolower(client_message[i]);
				}
				cout << "Заказ пользователя: " << client_message << endl;

				cout << "Очередь: " << (isNext ? "Открыта" : "Закрыта") << endl;
				if (!isNext) {
					send(client_socket[i], queueNotificationMsg, strlen(queueNotificationMsg), 0);
					continue;
				}
				isNext = false;

				string check_exit = client_message;
				if (check_exit == "off") {
					cout << "Client #" << i << " is off\n";
					client_socket[i] = 0;
				}

				string temp = client_message;
				// temp += "\n";
				orders.push_back(temp);

				// Время паузы
				int PauseTime = 0;
				int tempPrice = 0;

				if (client_socket[i] != 0) {
					send(client_socket[i], orderProcessingMsg, strlen(orderProcessingMsg), 0);

					// Разбиваем строку пользователя, на элементы массива
					int size = 0;
					string* arrayWords = getWordArray(temp, size);
					for (int i = 0; i < size; i++) {
						cout << arrayWords[i] << endl;
					}

					// Перебор слов пользователя
					for (int i = 0; i < size; i++) {
						// Перебор слов меню
						for (int j = 0; j < quantity; j++) {
							if (arrayWords[i] == item[j]) {
								cout << "TRUE" << endl;

								cout << "Item: " << item[j] << endl;
								cout << "Time: " << productTime[j] << endl;
								cout << "Price: " << price[j] << endl;

								tempPrice += price[j];
								PauseTime += productTime[j];
							}
						}
					}

					string tempMsg;

					tempMsg = "Ваш заказ будет готов через: " + to_string(PauseTime) + " cекунд";
					strcpy_s(client_message, sizeof(client_message), tempMsg.c_str());
					send(s, client_message, strlen(client_message), 0);

					Sleep(PauseTime * 1000);
					isNext = true;

					tempMsg = "Ваш заказ готов. Он состовляет: " + to_string(tempPrice) + "$";
					strcpy_s(client_message, sizeof(client_message), tempMsg.c_str());
					send(s, client_message, strlen(client_message), 0);
				}

			}
			
		}
	}

	WSACleanup();

    return 0;
}

string* getWordArray(string text, int& size) {
	const int lengthText = text.length();
	int arrayCount = 0;
	int wordCount = 1;

	// Считаем кол-во слов
	for (int i = 0; i < lengthText; i++) {
		if (text[i] == ' ')
			wordCount++;
	}

	// Массив который соответствует кол-во слов
	string* arrayWords = new string[wordCount];

	// Добавляем по символьно слова в массив
	for (int i = 0; i < lengthText; i++) {
		if (text[i] != ' ')
			arrayWords[arrayCount] += text[i];
		else
			arrayCount++;
	}

	size = wordCount;

	// Очистка от знаков
	for (int i = 0; i < wordCount; i++) {
		string temp;

		for (int j = 0; j < arrayWords[i].length(); j++) {
			if (arrayWords[i][j] != ',' && arrayWords[i][j] != '.')
				temp += arrayWords[i][j];
		}
		
		arrayWords[i] = temp;
		// cout << arrayWords[i] << endl;
	}
	
	return arrayWords;
}