#pragma once

#include "header.h"

const int MAXSTRLEN = 1000;

class mainDlg {
public:
	mainDlg(void);
	~mainDlg(void);
	static BOOL CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
	BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	static mainDlg* ptr;
	void Cls_OnClose(HWND hwnd);
	void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

	void Socket();
	bool SendData(char*); //метод для отправки данных в сеть
	bool ReceiveData(char*, int);//метод для получения данных
	void CloseConnection(); //метод для закрытия соединения
	void SendDataMessage();//метод для отправки сообщения пользователя

	HWND hEdit1, hList1, hButton1;

	// Создание буфера
	char receiveMessage[MAXSTRLEN];
	char sendMessage[MAXSTRLEN];
};