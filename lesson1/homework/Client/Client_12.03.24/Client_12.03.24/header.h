#pragma once

#include <WS2tcpip.h>
#include <windows.h>
#include <windowsX.h>
#include <tchar.h>
#include <fstream>
#include <commctrl.h>
#include <iostream>
#include <string>
#include "resource.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")