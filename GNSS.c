#include "GNSS.h"

struct UTC{
    uint16_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
};

struct Position{
    uint16_t latitude_ddd;
    uint8_t latitude_mm;
    float latitude_pt_mm;
    
    char N_S;

    uint16_t longitude_ddd;
    uint8_t longitude_mm;
    float longitude_pt_mm;
    
    char E_W;

    int16_t height_above_sea_level;
    int16_t height_geoid;
};

struct UTC utc = {0,0,0,0,0,0};
struct Position pos = {0,0,0.0,' ',0,0,0.0,' ',0,0};

uint8_t active_satellite_num = 0;
bool position_available = false;
bool utc_available = false;

char Buffer[GNSS_BUFFER_LEN];
char *BufPtr;

static void ReadToComma(char *valstr){
    do{ *(valstr++) = *BufPtr++; }while(*(valstr - 1) != ',');
}

static void dddmm_mmm(char* valstr,uint16_t *ddd, uint8_t *mm,float *_mmm){
    char *ptr = valstr;
    for(;*ptr != '.';ptr++);
    if(ptr - 1 >= valstr)*mm = (*(ptr - 1) - '0');
    if(ptr - 2 >= valstr)*mm += (*(ptr - 2) - '0') * 10;
    if(ptr - 3 >= valstr)*ddd = (*(ptr - 3) - '0');
    if(ptr - 4 >= valstr)*ddd += (*(ptr - 4) - '0') * 10;
    if(ptr - 5 >= valstr)*ddd += (*(ptr - 5) - '0') * 100;

    *_mmm = 0;
    for(float t = 0.1;*++ptr != ',';t *= 0.1)*_mmm += (*ptr - '0') * t;
}

static void height(char* valstr,int16_t *height){
	*height = 0;
    char *ptr = valstr;
    for(;*ptr != ',';ptr++);
    for(uint16_t t = 1;--ptr != valstr;t *= 10)*height += (*ptr - '0') * t;
}

static void GNSS_Update(){
	BufPtr = &Buffer[0];
	char valstr[15];
	ReadToComma(valstr);

	if(valstr[3] == 'G' && valstr[4] == 'G' && valstr[5] == 'A'){	//GPGGA
		ReadToComma(valstr);
		if(valstr[0] != ','){
		    utc.hour 	= 10 * (valstr[0] - '0') + (valstr[1] - '0');
		    utc.minutes	= 10 * (valstr[2] - '0') + (valstr[3] - '0');
		    utc.seconds	= 10 * (valstr[4] - '0') + (valstr[5] - '0');
		}

		ReadToComma(valstr);
		if(valstr[0] != ',')dddmm_mmm(valstr,&(pos.latitude_ddd),&(pos.latitude_mm),&(pos.latitude_pt_mm));

		ReadToComma(valstr);
		if(valstr[0] == 'N')pos.N_S = 'N';
		else if(valstr[0] == 'S')pos.N_S = 'S';

		ReadToComma(valstr);
		if(valstr[0] != ',')dddmm_mmm(valstr,&(pos.longitude_ddd),&(pos.longitude_mm),&(pos.longitude_pt_mm));

		ReadToComma(valstr);
		if(valstr[0] == 'E')pos.E_W = 'E';
		else if(valstr[0] == 'W')pos.E_W = 'W';

		ReadToComma(valstr);
		if(valstr[0] == '1')position_available = true;

		ReadToComma(valstr);
		if(valstr[0] != ','){
		    if(valstr[1] != ',')active_satellite_num = (valstr[0] - '0') * 10 + (valstr[1] - '0');
		    else active_satellite_num = (valstr[0] - '0');
		}

		ReadToComma(valstr);

		ReadToComma(valstr);
		if(valstr[0] != ',')height(valstr,&(pos.height_above_sea_level));

		ReadToComma(valstr);

		ReadToComma(valstr);
		if(valstr[0] != ',')height(valstr,&(pos.height_geoid));
	}

	if(valstr[3] == 'Z' && valstr[4] == 'D' && valstr[5] == 'A'){ //GPZDA
		ReadToComma(valstr);
		if(valstr[0] != ','){
		    utc.hour = 10 * (valstr[0] - '0') + (valstr[1] - '0');
		    utc.minutes = 10 * (valstr[2] - '0') + (valstr[3] - '0');
		    utc.seconds = 10 * (valstr[4] - '0') + (valstr[5] - '0');
		}

		ReadToComma(valstr);
		if(valstr[0] != ',')utc.date = (valstr[0] - '0') * 10 + (valstr[1] - '0');

		ReadToComma(valstr);
		if(valstr[0] != ',')utc.month = (valstr[0] - '0') * 10 + (valstr[1] - '0');

		ReadToComma(valstr);
		if(valstr[0] != ',')utc.year = (valstr[0] - '0') * 1000 + (valstr[1] - '0') * 100 + (valstr[2] - '0') * 10  + (valstr[3] - '0');

		utc_available = true;
	}
}

void GNSS_UART_Receive(char c){
	if(c == '$'){
		BufPtr = &Buffer[0];
	}

	*BufPtr++ = c;

	if(c == '\n'){
		*BufPtr++ = '\0';
		GNSS_Update();
	}
}

void GNSS_UTC(uint16_t *year,uint8_t *month, uint8_t *date, uint8_t *hour, uint8_t *minutes, uint8_t *seconds){
	*year 		= utc.year;
	*month 		= utc.month;
	*date 		= utc.date;
	*hour 		= utc.hour;
	*minutes 	= utc.minutes;
	*seconds	= utc.seconds;
}

bool GNSS_UTC_Ready(){
	return utc_available;
}

bool GNSS_Position_Ready(){
	return position_available;
}

void GNSS_Position_DMM(char *n_s,double *lat,char *e_w, double *lon){
	*n_s 		= pos.N_S;
	*e_w 		= pos.E_W;
	*lat 		= (double)(pos.latitude_ddd * 100 + pos.latitude_mm) + pos.latitude_pt_mm;
	*lon 		= (double)(pos.longitude_ddd * 100 + pos.longitude_mm) + pos.longitude_pt_mm;
}

void GNSS_Position_DDD(char *n_s,double *lat,char *e_w, double *lon){
	*n_s 		= pos.N_S;
	*e_w 		= pos.E_W;
	*lat 		= (double)pos.latitude_ddd + ((double)pos.latitude_mm + pos.latitude_pt_mm) / 60.0;
	*lon 		= (double)pos.longitude_ddd + ((double)pos.longitude_mm + pos.longitude_pt_mm) / 60.0;
}

void GNSS_Position_DMS(char *n_s,uint16_t *lat_ddd,uint8_t *lat_mm,float *lat_ss,char *e_w,uint16_t *lon_ddd,uint8_t *lon_mm,float *lon_ss){
	*n_s 		= pos.N_S;
	*e_w 		= pos.E_W;
	*lat_ddd 	= pos.latitude_ddd;
	*lat_mm 	= pos.latitude_mm;
	*lat_ss 	= pos.latitude_pt_mm * 60.0;
	*lon_ddd 	= pos.longitude_ddd;
	*lon_mm 	= pos.longitude_mm;
	*lon_ss 	= pos.longitude_pt_mm * 60.0;
}

long GNSS_Height(){
	return pos.height_above_sea_level - pos.height_geoid;
}
