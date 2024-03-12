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

	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(_T("Starting server...")));

	// �������� ������
	mainDlg::Socket();

	// ��������� ��������� ������� IPv4
	addr.sin_family = AF_INET;
	// ����������� ����� "0.0.0.0"� ���������� ��������� �������� �������, ��������� ��������� � ���� sin_addr
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	//��������� ����. ������� htons ��������� �������������� ����� � ������� ������� ����
	addr.sin_port = htons(port);

	wsprintf(buff, _T("Binding to port: %d"), port);
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(buff));
	
	if (bind(_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(_T("Failed to bind to port\n")));
		WSACleanup();
	}


	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(_T("Waiting for client...")));
	if (listen(_socket, 1) == SOCKET_ERROR) {
		SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(_T("Listening error\n")));
		WSACleanup();
	}

	// ����� �������� �����������. ������� ����������� �������. ��� ������ ������ �����������, ������� accept ���������� ����� �����, ����� ������� ���������� ����� �������.
	acceptSocket = accept(_socket, NULL, NULL);
	while (acceptSocket == SOCKET_ERROR) {
		acceptSocket = accept(_socket, NULL, NULL);
	}
	_socket = acceptSocket;
	
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

void mainDlg::Cls_OnClose(HWND hwnd) {
	EndDialog(hwnd, 0);

	// ������� �������
	WSACleanup();
}

void mainDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	if (id == IDC_BUTTON1) {
		SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(_T("\tWaiting...")));

		ReceiveData(receiveMessage, MAXSTRLEN);
		SendDataMessage();

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