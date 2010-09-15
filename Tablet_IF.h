#define WACOMTOOLTYPE_NONE      0x00
#define WACOMTOOLTYPE_PEN       0x01
#define WACOMTOOLTYPE_ERASER    0x04

#define WACOMBUTTON_TOUCH       5
#define WACOMBUTTON_STYLUS      6
#define WACOMBUTTON_STYLUS2     7

#define WACOMFIELD_TOOLTYPE     0
#define WACOMFIELD_PROXIMITY    2
#define WACOMFIELD_BUTTONS      3
#define WACOMFIELD_POSITION_X   4
#define WACOMFIELD_POSITION_Y   5
#define WACOMFIELD_PRESSURE		8

#define WACOMFIELD_MAX          14

typedef struct
{
	int nValue;
	int nMin;
	int nMax;
	int nReserved;
} TABLETVALUE;

typedef struct
{
	TABLETVALUE values[WACOMFIELD_MAX];
} TABLETSTATE;

#define BIT(x) (1<<(x))

extern TABLETSTATE State;

void PrintStatus();

extern int TabletPC_Parse(const unsigned char* puchData, unsigned int uLength);

HANDLE SerialInit(LPCWSTR comport, int baudrate);
DWORD SerialReadRaw(HANDLE fSerial, unsigned char* puchData, unsigned int uSize);