#include "project_cfg.h" 
#ifdef  LED_MODULE
#ifndef  _LED_DATA_H_
#define _LED_DATA_H_

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
#if 0
/* the slave device address: LP8860 */
#define LED_SLAVE_ADDRESS_WRITE 0x5A
#define LED_SLAVE_ADDRESS_READ  0x5B
#endif

/* the slave device address: LP8863 */
#define LED_SLAVE_ADDRESS_WRITE 0x58
#define LED_SLAVE_ADDRESS_READ  0x59


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
extern void LED_Data_I2C_Init(void);
extern void LED_Data_Cfg_Init(void);
extern void LED_Data_Set_PWM(UINT8 pwm_level);
extern UINT8 LED_Data_Fault_Status_Read(void);
extern void LED_Data_Fault_Msg_Clear(void);
extern UINT16 LED_Data_Fault_Msg_Read(void);
extern void LED_Data_Power_Set(UINT8 off_on);
extern UINT8 LED_Data_Power_En_Check(void);
extern void LED_Data_Fault_Cmd_Make(UINT16 fault_msg);

#ifdef __cplusplus
}
#endif

#endif  /* _LED_DATA_H_ */
#endif /* LED_MODULE */
