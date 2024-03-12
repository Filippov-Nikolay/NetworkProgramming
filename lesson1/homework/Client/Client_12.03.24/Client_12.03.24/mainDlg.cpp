#pragma once
#include "mainDlg.h"

mainDlg* mainDlg::ptr = NULL;

// ��������� ��� �������� ���������� � ������������� �������
WSADATA wsaData;
// ���������� ���������� ������
SOCKET _socket;
// ���������� ������, ������� ������ � �������� 
SOCKET acceptSocket;
// ��������� ����� � ����
sockaddr_in addr;

// ����� �����
int port = 25565;

bool fFirst = true;

mainDlg::mainDlg(void) {
	ptr = this;
}

void mainDlg::Socket() {
	//���� ������������� ������� ������ ���������, ������� ��������� ��
	//������
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(_T("WSAStartup error\n")));
		WSACleanup();
	}

	//������� ��������� �����, �������� TCP
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//��� ���������� �������� ������ ������� ��������� �� ������
	if (_socket == INVALID_SOCKET) {
		SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(_T("Socket create error.\n")));
		WSACleanup();
	}
}

bool mainDlg::SendData(char* buffer) {
	// ���������� ��������� �� ��������� �����
	send(_socket, buffer, strlen(buffer), 0);
	return true;
}

bool mainDlg::ReceiveData(char* buffer, int size) {
	// �������� ��������� � ��������� ��� � ������. ����� �������� �����������!
	int i = recv(_socket, buffer, size, 0);
	buffer[i] = '\0';

	_TCHAR buff[MAXSTRLEN];

	int len = MultiByteToWideChar(CP_ACP, 0, buffer, -1, NULL, 0);
	wchar_t* wbuffer = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, buffer, -1, wbuffer, len);

	wsprintf(buff, _T("Client msg: %s"), wbuffer);
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(buff));

	return true;
}

void mainDlg::CloseConnection() {
	//��������� �����
	closesocket(_socket);
}

mainDlg::~mainDlg(void) {
}

BOOL mainDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
	_TCHAR buff[MAX_PATH] = _T("");

	hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);
	hList1 = GetDlgItem(hwnd, IDC_LIST1);
	hButton1 = GetDlgItem(hwnd, IDC_BUTTON1);

	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(_T("Enter an IP address:")));

	// �������� ������
	mainDlg::Socket();

	return 0;
}

void mainDlg::SendDataMessage() {
	//������ ��� ��������� ������������
	_TCHAR buff[MAXSTRLEN];
	_TCHAR str[MAXSTRLEN];
	char message[MAXSTRLEN];
	//��� ����� ������ �� ������ ����� ������ ���������
	//���� ������������, ��������� �����.
	GetWindowText(hEdit1, buff, GetWindowTextLength(hEdit1) + 1);

	wsprintf(str, _T("Your msg: %s"), buff);
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(str));

	for (int i = 0; i < _tcslen(buff); i++) {
		message[i] = buff[i];

		if (i == _tcslen(buff) - 1)
			message[i + 1] = '\0';
	}

	SendData(message);
}

void mainDlg::ConnectToServer(const char* ipAddress, int port) {
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, ipAddress, &addr.sin_addr);
	addr.sin_port = htons(port);

	//��� ��������� ����������� � ������� ������� ��������� ��� ������
	if (connect(_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		_TCHAR buff[MAXSTRLEN];

		SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(_T("Failed to connect to server\n")));
		WSACleanup();
	}
}

void mainDlg::Cls_OnClose(HWND hwnd) {
	EndDialog(hwnd, 0);

	// ������� �������
	WSACleanup();
}

void mainDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	if (id == IDC_BUTTON1) {
		_TCHAR buff[MAXSTRLEN];

		if (fFirst)
			SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(_T("Enter an IP address:")));
		
		GetWindowText(hEdit1, buff, GetWindowTextLength(hEdit1) + 1);

		if (!fFirst) {
			SendDataMessage();
			ReceiveData(receiveMessage, MAXSTRLEN);
		}

		if (fFirst) {
			fFirst = false;
			char ipAddress[MAXSTRLEN];
			for (int i = 0; i < _tcslen(buff); i++) {
				ipAddress[i] = buff[i];

				if (i == _tcslen(buff) - 1)
					ipAddress[i + 1] = '\0';
			}
			ConnectToServer(ipAddress, port);
		}

		SetWindowText(hEdit1, _T(""));
	}
}

BOOL CALLBACK mainDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}

	return FALSE;
}