#include "project_cfg.h" 
#ifdef  CTP_MODULE
#ifndef  _CTP_CTRL_H_
#define _CTP_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************								
*  Name: .c
*  Description: 
*  Project: C_demo
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/		

/**********************************************************************************************
* External objects
**********************************************************************************************/
#include "derivative.h" 

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define CTP_IC  I2C1
#define COORDINATE_BUF_NUMBER 100
/**********************************************************************************************
* Global variables
**********************************************************************************************/
typedef struct
{
    UINT8 write_pos;
    UINT8 read_pos;
    UINT8 buf[COORDINATE_BUF_NUMBER][6];
    UINT8 action;
}TOUCH_COORDINATE_BUF;

/**********************************************************************************************
* Local types
**********************************************************************************************/


/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void CTP_Ctrl_Init(void);
extern void CTP_Ctrl_Standby_Set(UINT8 standby_state);

extern void CTP_Ctrl_Touch_Init(void);

extern void CTP_Ctrl_Task(void);

extern void CTP_Ctrl_Cmd_Make(void);

extern void CTP_Ctrl_Cmd_Send(UINT8* data, UINT8 data_size);

extern void CTP_Ctrl_Des_Set_Cds(UINT8 level);

#ifdef __cplusplus
}
#endif

#endif /* _CTP_CTRL_H_ */
#endif /* CTP_MODULE */
