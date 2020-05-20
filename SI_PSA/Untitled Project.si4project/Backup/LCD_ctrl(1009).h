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
#ifdef LCD_MODULE

#ifndef _LCD_CTRL_H_
#define _LCD_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************
* Include
**********************************************************************************************/
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
* Local types
**********************************************************************************************/


/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/


/**********************************************************************************************
* Local functions
**********************************************************************************************/


/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void LCD_Ctrl_Init(void);
extern void LCD_Ctrl_Deinit(void);
extern void LCD_Ctrl_Standby_Set(uint8_t standby_state);
extern uint8_t LCD_Ctrl_Fault_Detect(void);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_CTRL_H_ */
#endif /* LCD_MODULE */
