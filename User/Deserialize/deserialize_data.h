#include "project_cfg.h" 
#ifdef  DESERIALIZE_MODULE
#ifndef _DESERIALIZE_DATA_H_
#define _DESERIALIZE_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************								
*  Name: deserialize_data.h
*  Description: 
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
#include "deserialize_uart.h" 
typedef struct
{
    UINT8 backlight;
    UINT8 update_flag;
} DESERIALIZE_BLACK_STA;
extern DESERIALIZE_BLACK_STA Deserialize_Data_Backlight_sta;

/*******************************************************************************
* Global Functions
********************************************************************************/
extern void Deserialize_Data_Init(void);
extern void Deserialize_Data_Send(UINT8* data, UINT8 data_size);
extern UINT8 Deserialize_Data_Send_En(void);
extern void Deserialize_Data_Set_Cds(UINT8 level);
extern void Deserialize_Data_Set_Backlight(UINT8 value,UINT8 flag);

#ifdef __cplusplus
}
#endif

#endif /* _DESERIALIZE_DATA_H_ */
#endif /* DESERIALIZE_MODULE */

