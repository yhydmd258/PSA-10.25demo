#include "project_cfg.h" 
#ifdef  DESERIALIZE_MODULE
#ifndef _DESERIALIZE_IF_H_
#define  _DESERIALIZE_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************								
*  Name: Main.c
*  Description: 
*  Project: C_demo
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/		
#include "derivative.h"
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/


/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void Deserialize_If_Init(void);

extern void Deserialize_If_Cmd_Send(UINT8* data, UINT8 data_size);

extern UINT8 Deserialize_If_Send_En(void);

extern void Deserialize_If_Set_Cds(UINT8 level);

extern UINT8 Deserialize_If_Get_Backlight(void);

extern UINT8 Deserialize_If_Backlight_Flag(void);

extern void Deserialize_If_Backlight_Flag_update(UINT8 sta);


#ifdef __cplusplus
}
#endif

#endif/* _DESERIALIZE_IF_H_ */

#endif/*DESERIALIZE_MODULE*/
