#include <iostream>
#include<Windows.h>
using namespace std;

int main()
{
	/*system("color 0A");
	cout << "Hello world";

	system("color 0D");
	cout << "Hello world";*/

	COORD point{0, 0};// Координата точки
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); // получаем адрес собственного окна
	
	
	SetConsoleCursorPosition(h, point); // установка курсора в указанную позицию!
	SetConsoleTextAttribute(h, 12); // изменение цвета шрифта
	cout << "Hello world";    //вывод текста в указанной позиции

	// замена координат точки
	//point.Y += 1; // строки
	point.X += 12;  // столбцы! 
	SetConsoleCursorPosition(h, point); /// установка курсора в указанную позицию!
	SetConsoleTextAttribute(h, 14);  // изменение цвета шрифта
	cout << "Hello world";    //вывод текста в указанной позиции

	point.Y+=1;  // переход на новую строку!
	point.X = 0; // начало строки
	SetConsoleCursorPosition(h, point);
	SetConsoleTextAttribute(h, 11);
	cout << "Hello world\n";

	SetConsoleTextAttribute(h, 7); // стандарный цвет!

}