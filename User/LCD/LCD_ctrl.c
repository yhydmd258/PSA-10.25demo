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
/**********************************************************************************************
* External objects
**********************************************************************************************/
#include <string.h>
#include "../Command/command.h"
#include "../Timer/timer_if.h"
#include "gpio.h"
#include "LCD_if.h" 
#include "LCD_ctrl.h"
#include "LCD_data.h"

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/

/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef struct
{
    uint8_t reg_address;
    uint8_t reg_data;
}LCD_REG_SET;


/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/***********************************************************************************************
*
* @brief    init LCD module
* @param    none
* @return   none
*
************************************************************************************************/
void LCD_Ctrl_Init(void)
{
    /* init SPI */
    LCD_Data_Init();
    /* set register data */
   LCD_Ctrl_Register_Set();
    /*set standby GPIO High*/
    LCD_Data_GPIO_Standby_High();
}

/***********************************************************************************************
*
* @brief    Deinit SPI module
* @param    none
* @return   none
*
************************************************************************************************/
void LCD_Ctrl_Deinit(void)
{
    LCD_Data_Deinit();
}

/***********************************************************************************************
*
* @brief    init standby
* @param    none
* @return   none
*
************************************************************************************************/
void LCD_Ctrl_Standby_Set(uint8_t standby_state)
{

}

/***********************************************************************************************
*
* @brief    detect the error pin
* @param    none
* @return   1: error
                 0: normal
*
************************************************************************************************/
uint8_t LCD_Ctrl_Fault_Detect(void)
{
    return LCD_Data_Fault_Detect();
}

/***********************************************************************************************
*
* @brief    set register of LCD IC
* @param    none
* @return   none
*
************************************************************************************************/
void LCD_Ctrl_Register_Set(void)
{
    /* LCD IC: RM5365B */
    LCD_REG_SET reg_set[] ={{0x00,0x0D},{0x1F,0xaa},{0x19,0x05},{0x18,0x16},{0x17,0x01}};   
    uint32_t reg_set_size = sizeof(reg_set)/sizeof(LCD_REG_SET);
    uint32_t i;
	uint8_t data;    
    i=0x000b0000;/*90.2ms*/
    while(i--);//  delay 
    /* write data to LCD IC */
    for(i=0; i<reg_set_size; i++)
    {
        LCD_Data_Register_Write(LCD_SID_MASTER, reg_set[i].reg_address, reg_set[i].reg_data);
    }
	
#if 1
    /* read data from LCD IC */
    for(i=0; i<reg_set_size; i++)
    {
        if(reg_set[i].reg_address == LCD_IC_PAGE_REG)
        {
            LCD_Data_Register_Write(LCD_SID_MASTER, reg_set[i].reg_address, reg_set[i].reg_data);
        }
        else
        {
            LCD_Data_Register_Read(LCD_SID_MASTER, reg_set[i].reg_address, &data);
//            if(data != reg_set[i].reg_data)
//                break;
        }
    }
#endif

}

#endif /* LCD_MODULE */
