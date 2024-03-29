#include <iostream>
#include <string>
#include "WinSock2.h" //����� ��������� ����������, �����������
//��� Winsock 2 API.
#include <ws2tcpip.h> //�������� ������� ��� ������ � ��������
//����. inet_pton
#pragma comment(lib, "Ws2_32.lib") //������� ���������� Windows Sockets

using namespace std;

void main()
{
	const int MAXSTRLEN = 255;
	WSADATA wsaData;//��������� ��� �������� ����������
					//� ������������� �������
	SOCKET _socket; //���������� ���������� ������
	SOCKET acceptSocket;//���������� ������, ������� ������ � �������� 
	sockaddr_in addr; //��������� ����� � ����
	//������������� �������
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//������� ��������� �����, �������� TCP
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//��������� ������� IPv4
	addr.sin_family = AF_INET;
	/*����������� ����� � ����������
	��������� �������� �������, ��������� ��������� � ���� sin_addr */
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	//��������� ����. 
	//������� htons ��������� �������������� ����� �
	//������� ������� ����, �.�. ����������� ������� 
	//�������� ������� �� �������� � ��������
	addr.sin_port = htons(20000);
	//������ ����� � ���������� ������ � �����
	bind(_socket, (SOCKADDR*)&addr, sizeof(addr));
	//��������� ����� � ������ ������������� �������� �����������
	listen(_socket, 1);
	cout << "Server is started\n";
	//������� ����������� �������.
	//� ������ ��������� ����������� - �������� ����� �����
	//����� ������� ����� ����������� ����� �����������
	//� ��������.
	//������� �������� �����������.
	acceptSocket = accept(_socket, NULL, NULL);
	
	char buf[MAXSTRLEN];
	char str[MAXSTRLEN];

	cout << "\nEnter text:\n";
	cin.getline(str, MAXSTRLEN);

	//���������� ��������� �������
	send(acceptSocket, str, strlen(str), 0);

	//�������� ��������� �� �������
	//������� �������� �����������
	int i = recv(acceptSocket, buf, MAXSTRLEN, 0);
	buf[i] = '\0';

	//��������� ������
	closesocket(acceptSocket);
	closesocket(_socket);
	//����������� �������
	WSACleanup();
	system("pause");
}