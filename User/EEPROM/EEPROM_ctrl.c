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
#include <string.h>
#include "../Timer/timer_if.h"
#include "../Command/command.h"

#include "EEPROM_if.h"
#include "EEPROM_ctrl.h"
#include "EEPROM_data.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define EEPROM_LCD_DATA_SIZE   3
typedef struct 
{
    uint8_t record_flag;
    UPGRADE_FLAG_STRUCT upgrade_flag;
    uint8_t lcd_data[EEPROM_LCD_DATA_SIZE];
    uint8_t ctp_config_flag;
}EEPROM_STRUCT_DATA;

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
EEPROM_STRUCT_DATA Eeprom_Struct_Data = {0};

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/***********************************************************************************************
*
* @brief    init the EEPROM module, this mudule is initialized after CTP init
* @param    none
* @return   none
*
************************************************************************************************/
void EEPROM_Ctrl_Init(void)
{
    /* read data from EEPROM */
    if(Eeprom_Struct_Data.record_flag == FALSE)
    {
        if(!EEPROM_Data_Random_Read(PERIPHERAL_EEPROM_START_ADDR, ((uint8_t*)&Eeprom_Struct_Data.upgrade_flag), sizeof(EEPROM_STRUCT_DATA)-1))
        {
            Eeprom_Struct_Data.record_flag = TRUE;
            /* send data to LCD */
            EEPROM_If_Data_Send(Eeprom_Struct_Data.lcd_data, EEPROM_LCD_DATA_SIZE, LCD_VCOM_ENUM);
        }
    }
}

/***********************************************************************************************
*
* @brief    read data according to data type
* @param   data_buf: the pointer of data
                data_size:  the size of data
                data_type: the type of read data
* @return   0: success
                1: error
*
************************************************************************************************/
uint8_t EEPROM_Ctrl_Read(uint8_t *data_buf, uint8_t *data_size, uint8_t data_type)
{
    uint8_t result=1;
    
    if(TRUE == Eeprom_Struct_Data.record_flag)
    {
        /* return the data according to the data type */
        switch(data_type)
        {
            case MCU_UPGRADE_STATUS_ENUM:
                *data_size = sizeof(UPGRADE_FLAG_STRUCT);
                memcpy(data_buf, (uint8_t*)&Eeprom_Struct_Data.upgrade_flag, *data_size);
                result=0;
                break;
            case LCD_VCOM_ENUM:
                *data_size = EEPROM_LCD_DATA_SIZE;
                memcpy(data_buf, Eeprom_Struct_Data.lcd_data, *data_size);
                result=0;
                break;
            default:
                result = 1;
                break;
        }
    }
    else
    {
        result = 1;
    }

    return result;
}
/***********************************************************************************************
*
* @brief    save the data in local database and EEPROM
* @param    none
* @return   none
*
************************************************************************************************/
void EEPROM_Ctrl_Save(uint8_t *data_buf, uint8_t data_size, uint8_t data_type)
{
    uint8_t local_data_size=0;
//    EEPROM_STRUCT_DATA  read ={0};
    
    /* save data according to data type */
    switch(data_type)
    {
        case MCU_UPGRADE_STATUS_ENUM:
            local_data_size = sizeof(UPGRADE_FLAG_STRUCT);
            local_data_size = (local_data_size>data_size)? data_size : local_data_size;
            memcpy((uint8_t*)&Eeprom_Struct_Data.upgrade_flag, data_buf, local_data_size);
            /* write the vcom data into the eeprom */
            EEPROM_Data_Random_Write(PERIPHERAL_EEPROM_START_ADDR, (uint8_t*)&Eeprom_Struct_Data.upgrade_flag, sizeof(EEPROM_STRUCT_DATA)-1);
            break;
        case LCD_VCOM_ENUM:
            local_data_size = EEPROM_LCD_DATA_SIZE;
            local_data_size = (local_data_size>data_size)? data_size : local_data_size;
            memcpy(Eeprom_Struct_Data.lcd_data, data_buf, local_data_size);
            /* write the vcom data into the eeprom */
            EEPROM_Data_Random_Write(PERIPHERAL_EEPROM_START_ADDR, (uint8_t*)&Eeprom_Struct_Data.upgrade_flag, sizeof(EEPROM_STRUCT_DATA)-1);

//            EEPROM_Data_Random_Read(0, (uint8_t*)&(read.upgrade_flag), sizeof(read)-1);
            break;
        default:
            break;
    }
}
#endif	/* EEPROM_MODULE */
