#define WACOMTOOLTYPE_NONE      0x00
#define WACOMTOOLTYPE_PEN       0x01
#define WACOMTOOLTYPE_ERASER    0x04

#define WACOMBUTTON_TOUCH       5
#define WACOMBUTTON_STYLUS      6
#define WACOMBUTTON_STYLUS2     7

typedef struct
{
	int proximity;
	int toolType;
	int posX;
	int posY;
	int pressure;
	int buttons;
} TABLETSTATE;

#define BIT(x) (1<<(x))

void PrintStatus(TABLETSTATE state);

TABLETSTATE TabletPC_Parse(const unsigned char* puchData, unsigned int uLength);

HANDLE SerialInit(LPCWSTR comport, int baudrate);
DWORD SerialReadRaw(HANDLE fSerial, unsigned char* puchData, unsigned int uSize);