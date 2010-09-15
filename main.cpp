#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "Tablet_IF.h"

int main()
{
	unsigned char szBuff[9] = {0};
	TABLETSTATE state;

	INPUT  Input={0};

	HANDLE hSerial;
	hSerial = SerialInit(L"COM2", CBR_19200);
	if (hSerial == (HANDLE)FALSE){ return -1; }

	while (1) {
		if (sizeof(szBuff)==SerialReadRaw(hSerial, szBuff, sizeof(szBuff)))
		{
			state = TabletPC_Parse(szBuff, sizeof(szBuff));
			PrintStatus(state);
			
			if (state.proximity)
			{
				::ZeroMemory(&Input,sizeof(INPUT));

				Input.type = INPUT_MOUSE;
				Input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
				Input.mi.dx = ceil(state.posX*2.15948333);
				Input.mi.dy = ceil(state.posY*3.45508224);
				::SendInput(1,&Input,sizeof(INPUT));
			}
			if (state.buttons & BIT(WACOMBUTTON_TOUCH))
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