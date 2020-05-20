#include "project_cfg.h" 
#ifdef  DESERIALIZE_MODULE
#ifndef _DESERIALIZE_UART_H_
#define _DESERIALIZE_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************								
*  Name: Deserialize_uart.h
*  Description: deserializer init, send data and get data.
*  Project: C_demo
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/		

/*******************************************************************************
* Includes
********************************************************************************/
#include "derivative.h" /* include peripheral declarations */

/*******************************************************************************
* Constants
********************************************************************************/


/*******************************************************************************
* Macros
********************************************************************************/
#define RECEIVE_MAX_SIZE 0xFF
                
/*******************************************************************************
* Types
********************************************************************************/
typedef struct
{
    UINT8 data_buf[RECEIVE_MAX_SIZE];
    UINT16 read_pos;
    UINT16 write_pos;
}DES_UART_RECEIVE_DATA;

/*******************************************************************************
* Variables
********************************************************************************/


/*******************************************************************************
* Global Variables
********************************************************************************/


/*******************************************************************************
* Global Functions
********************************************************************************/
extern void Deserialize_UART_Init(void);
extern DES_UART_RECEIVE_DATA* Deserialize_UART_Get_Data(void);
extern void Deserialize_UART_Send_Data(UINT8* data, UINT8 data_size);

#ifdef __cplusplus
}
#endif

#endif /* _DESERIALIZE_UART_H_ */
#endif /* DESERIALIZE_MODULE */
