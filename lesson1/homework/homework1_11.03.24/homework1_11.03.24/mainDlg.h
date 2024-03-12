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
	bool SendData(char*); //����� ��� �������� ������ � ����
	bool ReceiveData(char*, int);//����� ��� ��������� ������
	void CloseConnection(); //����� ��� �������� ����������
	void SendDataMessage();//����� ��� �������� ��������� ������������

	HWND hEdit1, hList1, hButton1;

	// �������� ������
	char receiveMessage[MAXSTRLEN];
	char sendMessage[MAXSTRLEN];
};