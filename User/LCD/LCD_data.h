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

#ifndef _LCD_DATA_H_
#define _LCD_DATA_H_

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
typedef enum
{
    LCD_SID_MASTER  =3
    ,LCD_SID_SLAVE1 =2
    ,LCD_SID_SLAVE2 =1
    ,LCD_SID_RESERVED
}LCD_IC_SID;
    
#define LCD_IC_PAGE_REG 0x00

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
extern void LCD_Data_Init(void);
extern void LCD_Data_Deinit(void);
extern void LCD_Data_GPIO_Standby_High(void);
extern void LCD_Data_Register_Write(LCD_IC_SID IC_SID, uint8_t reg, uint8_t register_data);
extern void LCD_Data_Register_Read( LCD_IC_SID IC_SID, uint8_t reg, uint8_t *register_data);
extern void LCD_Data_Standby(void);
extern uint8_t LCD_Data_Fault_Detect(void);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_DATA_H_ */
#endif /* LCD_MODULE */
