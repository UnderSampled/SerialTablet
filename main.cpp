#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "Tablet_IF.h"

int main()
{
	unsigned char szBuff[9] = {0};

	INPUT  Input={0};

	HANDLE hSerial;
	hSerial = SerialInit(L"COM2", CBR_19200);
	if (hSerial == (HANDLE)FALSE){ return -1; }

	while (1) {
		if (sizeof(szBuff)==SerialReadRaw(hSerial, szBuff, sizeof(szBuff)))
		{
			if (TabletPC_Parse(szBuff, sizeof(szBuff))){printf("Error: Length Incorrect\n");} else {printf("Successfully parsed tablet.\n");}
			PrintStatus();
			
			if (State.proximity)
			{
				::ZeroMemory(&Input,sizeof(INPUT));

				Input.type = INPUT_MOUSE;
				Input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
				Input.mi.dx = ceil(State.posX*2.15948333);
				Input.mi.dy = ceil(State.posY*3.45508224);
				::SendInput(1,&Input,sizeof(INPUT));
			}
			if (State.buttons & BIT(WACOMBUTTON_TOUCH))
			{
				//::ZeroMemory(&Input,sizeof(INPUT));

				//Input.type = INPUT_MOUSE;
				//Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;
				//::SendInput(1,&Input,sizeof(INPUT));
				printf("down\n");
			} else {
				//::ZeroMemory(&Input,sizeof(INPUT));

				//Input.type = INPUT_MOUSE;
				//Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
				//::SendInput(1,&Input,sizeof(INPUT));
				printf("up\n");
			}
		}
	}

	CloseHandle(hSerial);

	printf("Press Return to exit.\n");
	getchar();
	return 0;
}