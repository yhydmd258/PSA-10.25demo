/******************************************************************************
*  Name: .c
*  Description:
*  Project: SGM
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/
#include "project_cfg.h" 
#ifdef EEPROM_MODULE

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
extern void EEPROM_If_Init(void);
extern void EEPROM_If_Data_Send(uint8_t *data_buf, uint8_t data_size, uint8_t data_type);
extern void EEPROM_If_Data_Save(uint8_t *data_buf, uint8_t data_size, uint8_t data_type);
extern uint8_t EEPROM_If_Data_Read(uint8_t *data_buf, uint8_t *data_size, uint8_t data_type);

#endif	/* EEPROM_MODULE */
