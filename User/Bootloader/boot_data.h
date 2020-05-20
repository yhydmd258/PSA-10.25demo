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
#ifdef  BOOTLOADER

#ifndef _BOOT_DATA_H_
#define _BOOT_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

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
#define BOOT_BUF_LENGTH_POS  0x00
#define BOOT_SEND_BUF_NUMBER 10
#define BOOT_SEND_BUF_SIZE  60
#define BOOT_REC_BUF_NUMBER 10
#define BOOT_REC_BUF_SIZE  260
#define BOOT_BUF_LENGTH_POS_SIZE    1
#define BOOR_DESER_RECEIVE_SEPARATOR  0x1C /* after separator is EXT_len and checksum */

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
extern void Boot_Data_Init(void);
extern void Boot_Data_Deinit(void);
extern void Boot_Data_Cmd_Read(uint8_t *cmd_data, uint32_t *cmd_size);
extern void Boot_Data_Send_Cmd_Save(uint8_t* send_data, uint32_t data_size);
extern uint16_t Boot_Data_App_Image_Erase(void);
extern uint16_t Boot_Data_App_Image_Write(uint32_t u32NVMTargetAddress, uint8_t *pData, uint16_t u16SizeBytes);
extern void Boot_Data_Interrupt_Set(uint8_t pin_level);
extern uint8_t Boot_Data_Interrupt_Low_Check(void);
extern uint8_t Boot_Data_Eeprom_Random_Write(uint32_t data_addr,uint8_t *send_data, uint32_t data_size);
extern uint8_t Boot_Data_Eeprom_Random_Read(uint8_t data_addr, uint8_t *read_data, uint32_t data_size);

#ifdef __cplusplus
}
#endif

#endif /* _BOOT_DATA_H_ */

#endif /* BOOTLOADER */

