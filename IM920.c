#include "IM920.h"

IM920_Setting setting;

#include "string.h"

uint8_t MssgBuffer[IM920_MSSG_BUFFER_LEN];
uint8_t RespBuffer[IM920_RESP_BUFFER_LEN];

uint16_t MssgBufferCount = 0;
uint16_t RespBufferCount = 0;

uint8_t RespCount = 0;
uint8_t MssgCount = 0;

volatile bool waitingResp = false;
volatile bool waitingMssg = false;

void UartWriteMulti(uint8_t *data,uint8_t len){
}

static bool CheckBusy(){
	bool isBusy;

	return isBusy;
}

void IM920_Reset(){
}

void IM920_UART_Receive(uint8_t c){
	if(waitingResp){
		RespBuffer[RespBufferCount++] = c;
		if(c == '\n')RespCount++;
	}else if(waitingMssg){
		MssgBuffer[MssgBufferCount++] = c;
		if(c == '\n')MssgCount++;
	}
}

static uint8_t ConvChar16ToInt16(char c){
	if('0' <= c && c <= '9')return c - '0';
	else return 10 + (c - 'A');
}

static uint16_t ConvStrToInt16(uint8_t *ptr){
	uint16_t t = 1;
	uint16_t res = 0;

	uint8_t i;
	for(i = 0;('0' <= ptr[i] && ptr[i] <= '9') || ('A' <= ptr[i] && ptr[i] <= 'F');i++);
	for(;i > 0;i--){
		res += ConvChar16ToInt16(ptr[i - 1]) * t;
		t *= 16;
	}
	return res;
}

static char ConvInt16ToChar16(uint8_t t){
    if(t <= 9)return '0' + t;
    else return 'A' - 10 + t;
}

static void StartReceive(){
	memset(MssgBuffer,0,IM920_MSSG_BUFFER_LEN);
	MssgBufferCount = 0;
	MssgCount = 0;
	waitingMssg = true;
}

static void WaitResponce(){
	memset(RespBuffer,0,IM920_RESP_BUFFER_LEN);
	RespBufferCount = 0;
	RespCount = 0;
	waitingResp = true;
}

static bool ReadParam(const char* cmd,uint16_t *param){
	if(CheckBusy())return false;

	uint8_t _cmd[] = {cmd[0],cmd[1],cmd[2],cmd[3],'\r','\n'};
	UartWriteMulti(_cmd,6);

	WaitResponce();
	while(RespCount<1);

	if(RespBuffer[0] == 'N' && RespBuffer[1] == 'G'){
		return false;
	}
	else{
		*param = ConvStrToInt16(RespBuffer);
		return true;
	}
}

static bool SetParam(const char* cmd,uint16_t param,uint8_t param_len){
	if(CheckBusy())return false;

	UartWriteMulti((uint8_t*)cmd,4);
	if(param_len == 2){
		uint8_t param_str[3];
		param_str[0] = ' ';
		param_str[1] = ConvInt16ToChar16(param%0x100/0x10);
		param_str[2] = ConvInt16ToChar16(param%0x10);
		UartWriteMulti(param_str,3);
	}else if(param_len == 4){
		uint8_t param_str[5];
		param_str[0] = ' ';
		param_str[1] = ConvInt16ToChar16(param%0x10000/0x1000);
		param_str[2] = ConvInt16ToChar16(param%0x1000/0x100);
		param_str[3] = ConvInt16ToChar16(param%0x100/0x10);
		param_str[4] = ConvInt16ToChar16(param%0x10);
		UartWriteMulti(param_str,5);
	}else if(param_len == 1){
		uint8_t param_str[2];
		param_str[0] = ' ';
		param_str[1] = param;
		UartWriteMulti(param_str,2);
	}
	UartWriteMulti((uint8_t*)"\r\n",2);

	WaitResponce();
	while(RespCount<1);

	if(RespBuffer[0] == 'N' && RespBuffer[1] == 'G')return false;
	else return true;
}

bool IM920_Initialize(){
	uint16_t temp;
	IM920_Reset();
	if(ReadParam("RDID",&temp))setting.ID = temp;
	else return false;
	if(ReadParam("RDNN",&temp))setting.NN = temp;
	else return false;
	if(ReadParam("RDCH",&temp))setting.CH = temp;
	else return false;
	SetParam("ECIO",0,0);
	StartReceive();
	return true;
}

bool IM920_SetReieveID(uint16_t ID){
	if(SetParam("ENWR",0,0)){
		return SetParam("SRID",ID,4);
	}else return false;
}

bool IM920_EraceReieveIDs(){
	if(SetParam("ENWR",0,0)){
		return SetParam("ERID",0,0);
	}else return false;
}

bool IM920_SetNodeNumber(uint8_t NodeNumber){
	return SetParam("STNN",NodeNumber,2);
}

bool IM920_SetChannel(uint8_t Channel){
	if(SetParam("ENWR",0,0)){
		return SetParam("STCH",Channel,2);
	}else return false;
}

bool IM920_SetRateMode(uint8_t Mode){
	return SetParam("STRT",Mode,1);
}

bool IM920_SetRelayMode(bool RelayMode){
	if(RelayMode)return SetParam("ERPT",0,0);
	else return SetParam("DRPT",0,0);
}

bool IM920_Sleep(){
	return SetParam("DSRX",0,0);
}

bool IM920_UnSleep(){
	UartWriteMulti((uint8_t*)"?",1);
	while(CheckBusy());
	return SetParam("ENRX",0,0);
}

bool IM920_Send(uint8_t *data,uint16_t len){
	if(CheckBusy())return false;

	UartWriteMulti((uint8_t*)"TXDA ",5);
	UartWriteMulti(data,len);
	UartWriteMulti((uint8_t*)"\r\n",2);

	WaitResponce();

	return true;
}

bool IM920_NewMessage(){
	return (MssgCount > 0);
}

void IM920_Read(uint8_t *data){
	if(MssgCount > 0)
		for(uint16_t i = 0;MssgBuffer[i] == '\0';i++)
			data[i] = MssgBuffer[i];
	return;
}
