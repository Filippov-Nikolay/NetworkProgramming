#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#pragma comment(lib, "urlmon.lib")
using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	// the URL to download from 
	// string srcURL = "https://bank.gov.ua/NBUStatService/v1/statdirectory/exchange?json";

	// string srcURL = "https://bank.gov.ua/NBUStatService/v1/statdirectory/exchange?valcode=USD&date=20240319&json";
	// the destination file 
	string destFile = "file.txt";

	string currency, exchange;
	cout << "Введите валюту (USD, EUR): ";
	cin >> currency;

	string year;
	cout << "Введите год: ";
	cin >> year;

	string month;
	cout << "Введите месяц: ";
	cin >> month;

	string day;
	cout << "Введите день: ";
	cin >> day;

	string date = (year += month += day);
	string srcURL = "https://bank.gov.ua/NBUStatService/v1/statdirectory/exchange?valcode=";
	srcURL += currency += "&date=";
	srcURL += date += "&json";

	cout << srcURL << endl;

	//// URLDownloadToFile returns S_OK on success 
	if (S_OK == URLDownloadToFileA(NULL, srcURL.c_str(), destFile.c_str(), 0, NULL))
		cout << "Saved to " << destFile << "\n";
	else cout << "Fail!\n";


	ifstream in(destFile, ios::app);
	string response, line;

	while (getline(in, line))
        response += line + "\n";

	cout << response << endl;
	
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

                if (strcmp(tempKey, "cc") == 0 || strcmp(tempKey, "exchangedate") == 0 || strcmp(tempKey, "rate") == 0) {
                    char tempValue[BUFFERSIZE];

                    for (int j = i + 4, k = 0; j < response.length(); j++) {
                        if (response[j] != '"' && response[j] != ',' && response[j] != '}') {
                            tempValue[k++] = response[j];
                        }
                        else {
                            tempValue[k] = '\0';

                            // cout << tempValue;

                            break;
                        }
                    }

                    if (strcmp(tempKey, "cc") == 0) {
                        currency = tempValue;
                        cout << "Валюта: " << tempValue << endl;
                    }
                    else if (strcmp(tempKey, "exchangedate") == 0) {
                        date = tempValue;
                        cout << "Дата: " << tempValue << endl;
                    }
                    else if (strcmp(tempKey, "rate") == 0) {
                        exchange = tempValue;
                        cout << "Курс: " << tempValue << endl;
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

    response = "Дата: " + date += "г, Валюта - " + currency + ", 1" + currency + " = " + exchange + "грн";

    cout << response << endl;

    ofstream out(destFile, ios::out);
    out << response;
    out.close();
    in.close();
}