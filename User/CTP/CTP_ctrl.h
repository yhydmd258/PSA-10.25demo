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
#include "project_cfg.h" 
#ifdef  CTP_MODULE

#ifndef  CTP_CTRL_H_
#define CTP_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************
* External objects
**********************************************************************************************/
#include "derivative.h" 

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

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
extern void CTP_Ctrl_Deinit(void);
extern void CTP_Ctrl_Status_Check(void);
extern uint8_t CTP_Ctrl_Fault_Check(void);
extern uint32_t CTP_Ctrl_Version_Read(void);
extern void CTP_Ctrl_Cmd_Analyse(uint8_t *cmd_buf, uint8_t cmd_size);
extern void CTP_Ctrl_Msg_Read(void);
extern void CTP_Ctrl_Cmd_Make(void);
extern void CTP_Ctrl_Cmd_Send_Save(uint8_t* data,uint8_t data_size,uint8_t repeat,uint8_t *extend_data,uint8_t extend_size);
extern void CTP_Ctrl_Int_Flag_Set(uint8_t flag);
extern uint8_t CTP_Ctrl_Int_Flag_Get(void);
extern void CTP_Ctrl_Cmd_Response(void);

#ifdef __cplusplus
}
#endif

#endif /* _CTP_CTRL_H_ */

#endif /* CTP_MODULE */
