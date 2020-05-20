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
 //   LCD_Ctrl_Register_Set();
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
    /* LCD IC: NT51625 */
    LCD_REG_SET reg_set[] ={{0x1E, 0x10},{0x01, 0x29},{0x02, 0x33},{0x03, 0x19},{0x04, 0x1C},
                            {0x05, 0x30},{0x06, 0x03},{0x07, 0x6F},{0x08, 0x3B},{0x09, 0x7F},
                            {0x0A, 0x0D},{0x0B, 0x12},{0x0C, 0xC4},{0x0D, 0xE2},{0x0E, 0x41},
                            {0x0F, 0x44},{0x10, 0x77},{0x11, 0x07},{0x12, 0x00},{0x13, 0x40},
                            {0x14, 0xC0},{0x15, 0x0B},{0x16, 0x20},{0x17, 0xE0},{0x18, 0x80},
                            {0x19, 0x77},{0x1A, 0xC3},
                            // Gamma code
                            {0x1B, 0xE0},{0x1C, 0x0E},{0x1D, 0x00},
                            {0x1E, 0x11},{0x00, 0xE0},{0x01, 0x0E},{0x02, 0x00},{0x03, 0xE0},
                            {0x04, 0x0E},{0x05, 0x00},{0x06, 0x00},{0x07, 0x00},{0x08, 0x00},
                            {0x09, 0x00},{0x0A, 0x00},{0x0B, 0x00},{0x0C, 0x00},{0x0D, 0x00},
                            {0x0E, 0x00},{0x0F, 0x00},{0x10, 0x00},{0x11, 0x00},{0x12, 0x00},
                            {0x13, 0x00},{0x14, 0x00},{0x15, 0x00},{0x16, 0x00},{0x17, 0x00},
                            {0x18, 0x00},{0x19, 0x00},{0x1A, 0x00},{0x1B, 0x00},{0x1C, 0x00},
                            {0x1D, 0x00},
                            {0x1E, 0x12},{0x00, 0x00},{0x01, 0x04},{0x02, 0x17},{0x03, 0x26},
                            {0x04, 0x32},{0x05, 0x3C},{0x06, 0x46},{0x07, 0x4E},{0x08, 0x56},
                            {0x09, 0xBE},{0x0A, 0xC5},{0x0B, 0xE2},{0x0C, 0xEF},{0x0D, 0xF5},
                            {0x0E, 0xEF},{0x0F, 0xF1},{0x10, 0xED},{0x11, 0xF5},{0x12, 0x03},
                            {0x13, 0x1D},{0x14, 0x23},{0x15, 0x99},{0x16, 0xA0},{0x17, 0xA8},
                            {0x18, 0xB0},{0x19, 0xBB},{0x1A, 0xC7},{0x1B, 0xD6},{0x1C, 0xEC},
                            {0x1D, 0xFE},
                            {0x1E, 0x13},{0x00, 0x00},{0x01, 0x04},{0x02, 0x17},{0x03, 0x26},
                            {0x04, 0x32},{0x05, 0x3C},{0x06, 0x46},{0x07, 0x4E},{0x08, 0x56},
                            {0x09, 0xBE},{0x0A, 0xC5},{0x0B, 0xE2},{0x0C, 0xEF},{0x0D, 0xF5},
                            {0x0E, 0xEF},{0x0F, 0xF1},{0x10, 0xED},{0x11, 0xF5},{0x12, 0x03},
                            {0x13, 0x1D},{0x14, 0x23},{0x15, 0x99},{0x16, 0xA0},{0x17, 0xA8},
                            {0x18, 0xB0},{0x19, 0xBB},{0x1A, 0xC7},{0x1B, 0xD6},{0x1C, 0xEC},
                            {0x1D, 0xFE},
                            {0x1E, 0x14},{0x00, 0x00},{0x01, 0x04},{0x02, 0x17},{0x03, 0x26},
                            {0x04, 0x32},{0x05, 0x3C},{0x06, 0x46},{0x07, 0x4E},{0x08, 0x56},
                            {0x09, 0xBE},{0x0A, 0xC5},{0x0B, 0xE2},{0x0C, 0xEF},{0x0D, 0xF5},
                            {0x0E, 0xEF},{0x0F, 0xF1},{0x10, 0xED},{0x11, 0xF5},{0x12, 0x03},
                            {0x13, 0x1D},{0x14, 0x23},{0x15, 0x99},{0x16, 0xA0},{0x17, 0xA8},
                            {0x18, 0xB0},{0x19, 0xBB},{0x1A, 0xC7},{0x1B, 0xD6},{0x1C, 0xEC},
                            {0x1D, 0xFE},
                            {0x1E, 0x15},{0x00, 0x00},{0x01, 0x04},{0x02, 0x17},{0x03, 0x26},
                            {0x04, 0x32},{0x05, 0x3C},{0x06, 0x46},{0x07, 0x4E},{0x08, 0x56},
                            {0x09, 0xBE},{0x0A, 0xC5},{0x0B, 0xE2},{0x0C, 0xEF},{0x0D, 0xF5},
                            {0x0E, 0xEF},{0x0F, 0xF1},{0x10, 0xED},{0x11, 0xF5},{0x12, 0x03},
                            {0x13, 0x1D},{0x14, 0x23},{0x15, 0x99},{0x16, 0xA0},{0x17, 0xA8},
                            {0x18, 0xB0},{0x19, 0xBB},{0x1A, 0xC7},{0x1B, 0xD6},{0x1C, 0xEC},
                            {0x1D, 0xFE},
                            {0x1E, 0x16},{0x00, 0x00},{0x01, 0x04},{0x02, 0x17},{0x03, 0x26},
                            {0x04, 0x32},{0x05, 0x3C},{0x06, 0x46},{0x07, 0x4E},{0x08, 0x56},
                            {0x09, 0xBE},{0x0A, 0xC5},{0x0B, 0xE2},{0x0C, 0xEF},{0x0D, 0xF5},
                            {0x0E, 0xEF},{0x0F, 0xF1},{0x10, 0xED},{0x11, 0xF5},{0x12, 0x03},
                            {0x13, 0x1D},{0x14, 0x23},{0x15, 0x99},{0x16, 0xA0},{0x17, 0xA8},
                            {0x18, 0xB0},{0x19, 0xBB},{0x1A, 0xC7},{0x1B, 0xD6},{0x1C, 0xEC},
                            {0x1D, 0xFE},
                            {0x1E, 0x17},{0x00, 0x00},{0x01, 0x04},{0x02, 0x17},{0x03, 0x26},
                            {0x04, 0x32},{0x05, 0x3C},{0x06, 0x46},{0x07, 0x4E},{0x08, 0x56},
                            {0x09, 0xBE},{0x0A, 0xC5},{0x0B, 0xE2},{0x0C, 0xEF},{0x0D, 0xF5},
                            {0x0E, 0xEF},{0x0F, 0xF1},{0x10, 0xED},{0x11, 0xF5},{0x12, 0x03},
                            {0x13, 0x1D},{0x14, 0x23},{0x15, 0x99},{0x16, 0xA0},{0x17, 0xA8},
                            {0x18, 0xB0},{0x19, 0xBB},{0x1A, 0xC7},{0x1B, 0xD6},{0x1C, 0xEC},
                            {0x1D, 0xFE}};

    LCD_REG_SET readback_enable_set[3] = {{0x1E, 0x20},{0x02,0x8F},{0x1D, 0x0A}};
    
    uint32_t reg_set_size = sizeof(reg_set)/sizeof(LCD_REG_SET);
    uint32_t readback_size = sizeof(readback_enable_set)/sizeof(LCD_REG_SET);
    uint32_t i;
    
    /* write data to LCD IC */
    for(i=0; i<reg_set_size; i++)
    {
        LCD_Data_Register_Write(LCD_SID_MASTER, reg_set[i].reg_address, reg_set[i].reg_data);
    }

    /* enable read */
    for(i=0; i<readback_size; i++)
    {
        LCD_Data_Register_Write(LCD_SID_MASTER, readback_enable_set[i].reg_address, readback_enable_set[i].reg_data);
    }
#ifdef DEBUG
//	 OUTPUT_CLEAR(PTB,PTB1);
	 OUTPUT_CLEAR(PTB,PTB5);
	 /* write data to LCD IC */
    for(i=0; i<reg_set_size; i++)
    {
        LCD_Data_Register_Write(LCD_SID_MASTER, 0x01,0x01);
    }
	OUTPUT_SET(PTB,PTB5);
//	OUTPUT_SET(PTB,PTB1);
#endif
#if 0

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
