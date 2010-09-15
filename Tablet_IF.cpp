#include <stdio.h>
#include <windows.h>

#include "Tablet_IF.h"

TABLETSTATE State = { 0 };

void PrintStatus()
{
	int tool = State.values[WACOMFIELD_TOOLTYPE].nValue;

	printf("Proximity: %d,	ToolType: %s,	Pos_X: %d,	Pos_Y: %d,	Pressure: %d,	Button: %0x\n", 
		State.values[WACOMFIELD_PROXIMITY].nValue,
		tool ? (tool == WACOMTOOLTYPE_PEN ? "Pen" : "Eraser") : "none",
		State.values[WACOMFIELD_POSITION_X].nValue,
		State.values[WACOMFIELD_POSITION_Y].nValue,
		State.values[WACOMFIELD_PRESSURE].nValue,
		State.values[WACOMFIELD_BUTTONS].nValue
	);
}

int TabletPC_Parse(const unsigned char* puchData, unsigned int uLength)
{
	int x=0, y=0, prox=0, tool=WACOMTOOLTYPE_NONE,
			button=0, press=0, eraser;

	/* Tablet PC Supports: 256 pressure, eraser, 1/2 side-switch */

	if (uLength != 9) { return 1; }

	prox = puchData[0] & 0x20 ? 1 : 0;
	if (prox)
	{
		eraser = (puchData[0] & 0x04) ? 1 : 0;
		press = ((puchData[6] & 0x01) << 7) | (puchData[5] & 0x7F);

		/* tools are distinguishable */
		if (eraser) tool = WACOMTOOLTYPE_ERASER;
		else tool = WACOMTOOLTYPE_PEN;
		
		button = (puchData[0] & 0x01) ? BIT(WACOMBUTTON_TOUCH) : 0;

		/* pen has side-switch(es), eraser has none */
		if (tool == WACOMTOOLTYPE_PEN)
		{
			button |= (puchData[0] & 0x02) ?
					BIT(WACOMBUTTON_STYLUS) : 0;
			button |= (puchData[0] & 0x04) ?
					BIT(WACOMBUTTON_STYLUS2) : 0;
		}

		x = (((int)puchData[6] & 0x60) >> 5) |
			((int)puchData[2] << 2) |
			((int)puchData[1] << 9);
		y = (((int)puchData[6] & 0x18) >> 3) |
			((int)puchData[4] << 2) |
			((int)puchData[3] << 9);
	}

	/* set valid fields */
	State.values[WACOMFIELD_PROXIMITY].nValue = prox;
	State.values[WACOMFIELD_TOOLTYPE].nValue = tool;
	State.values[WACOMFIELD_POSITION_X].nValue = x;
	State.values[WACOMFIELD_POSITION_Y].nValue = y;
	State.values[WACOMFIELD_PRESSURE].nValue = press;
	State.values[WACOMFIELD_BUTTONS].nValue = button;

	return 0;
}

HANDLE SerialInit(LPCWSTR comport, int baudrate)
{
	HANDLE hSerial;
	DWORD dwBytesWritten;

	hSerial = CreateFile(
			comport,
			GENERIC_READ | GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0
		);
	if(hSerial==INVALID_HANDLE_VALUE){
		if(GetLastError()==ERROR_FILE_NOT_FOUND){
			fprintf(stderr, "No serial port at %s\n", comport);
		}
			fprintf(stderr, "Unknown Error\n");
	}

	DCB dcbSerialParams = {0};

	dcbSerialParams.DCBlength=sizeof(dcbSerialParams);

	if (!GetCommState(hSerial, &dcbSerialParams)) {
		fprintf(stderr, "error getting state\n");
	}

	dcbSerialParams.BaudRate=baudrate;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;

	if(!SetCommState(hSerial, &dcbSerialParams)){
		fprintf(stderr, "error setting serial port state\n");
	}

	// Retrieve the timeout parameters for all read and write operations
	// on the port. 
	COMMTIMEOUTS CommTimeouts;
	GetCommTimeouts (hSerial, &CommTimeouts);

	// Change the COMMTIMEOUTS structure settings.
	CommTimeouts.ReadIntervalTimeout = MAXDWORD;  
	CommTimeouts.ReadTotalTimeoutMultiplier = 100;  
	CommTimeouts.ReadTotalTimeoutConstant = 10000;    
	CommTimeouts.WriteTotalTimeoutMultiplier = 10;  
	CommTimeouts.WriteTotalTimeoutConstant = 1000;    

	// Set the timeout parameters for all read and write operations
	// on the port. 
	if (!SetCommTimeouts (hSerial, &CommTimeouts))
	{
		// Could not set the timeout parameters.
		WCHAR lastError[1024];
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			lastError,
			1024,
			NULL);
		fprintf(stderr, "error occurred. Could not Read. %s\n", lastError);
		return (FILE*)FALSE;
	}

	// Tablet SPecific Init
	if(!WriteFile(hSerial, "1", 1, &dwBytesWritten, NULL)){ //get tablet to start communicating
		fprintf(stderr, "error occurred. Could not Write.\n");
	} else {
		printf("Wrote %d bytes.\n", dwBytesWritten);
	}

	return hSerial;
}

DWORD SerialReadRaw(HANDLE hSerial, unsigned char* puchData, unsigned int uSize)
{
	DWORD dwBytesRead = 0;
	unsigned int currentBytes;
	unsigned char* head;

	currentBytes = 0;

	if(!ReadFile(hSerial, puchData, uSize, &dwBytesRead, NULL)){
		//error occurred. Report to user.
		fprintf(stderr, "error occurred. Could not Read.\n");
	}
	for (head=puchData, currentBytes=dwBytesRead ; currentBytes ; currentBytes--)
	{
		if (*head & 0x80) break;
	}

	if (head!=puchData)
	{
		memcpy(puchData, head, currentBytes);
	}

	while (currentBytes < uSize)
	{
		if(!ReadFile(hSerial, puchData+currentBytes, uSize-currentBytes, &dwBytesRead, NULL)){
			//error occurred. Report to user.
			fprintf(stderr, "error occurred. Could not Read.\n");
		} else {
			currentBytes+=dwBytesRead;
		}
	}

	return currentBytes;
}