#include "project_cfg.h" 
#ifdef  DESERIALIZE_MODULE
#ifndef _DESERIALIZE_CTRL_H_
#define _DESERIALIZE_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************								
*  Name: deserialize_ctrl.h
*  Description: 
*  Project: C_demo
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/		
#include "derivative.h" /* include peripheral declarations */

/*******************************************************************************
* Includes
********************************************************************************/

/*******************************************************************************
* Global Functions
********************************************************************************/
extern void Deserialize_Ctrl_Init(void);
extern void Deserialize_Ctrl_Cmd_Send(UINT8* data, UINT8 data_size);
extern UINT8 Deserialize_Ctrl_Send_En(void);
extern void Deserialize_Ctrl_Set_Cds(UINT8 level);
extern UINT8 Deserialize_Ctrl_Get_Backlight(void);
extern UINT8 Deserialize_Ctrl_Backlight_Flag(void);
extern void Deserialize_Ctrl_Backlight_Flag_update(UINT8 sta);



#ifdef __cplusplus
}
#endif

#endif /* _DESERIALIZE_CTRL_H_ */
#endif /* DESERIALIZE_MODULE */

