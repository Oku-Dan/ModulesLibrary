#ifndef INC_GNSS_H_
#define INC_GNSS_H_

#include "stdbool.h"
#include "stdint.h"

#define GNSS_BUFFER_LEN 90

void GNSS_UART_Receive(char c);	//set this on UART rx interrupt

void GNSS_UTC(uint16_t *year,uint8_t *month, uint8_t *date, uint8_t *hour, uint8_t *minutes, uint8_t *seconds);
bool GNSS_UTC_Ready();
bool GNSS_Position_Ready();
void GNSS_Position_DMM(char *N_S,double *lat,char *E_W, double *lon);
void GNSS_Position_DDD(char *N_S,double *lat,char *E_W, double *lon);
void GNSS_Position_DMS(char *N_S,uint16_t *lat_ddd,uint8_t *lat_mm,float *lat_ss,char *E_W,uint16_t *lon_ddd,uint8_t *lon_mm,float *lon_ss);
long GNSS_Height();

#endif
