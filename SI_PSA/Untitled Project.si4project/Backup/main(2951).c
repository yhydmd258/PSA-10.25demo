/******************************************************************************								
*  Name: Main.c
*  Description: 
*  Project: C_demo
*  Auther:  shanghai tianma
*              R&D center
*              yanhaiyun
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/		
/**********************************************************************************************
* External objects
**********************************************************************************************/
/* user defined */
#include "project_cfg.h" /* this header file is must first */

#include <stdio.h>
/*Header file from MCU supplier*/
#include "derivative.h" /* include peripheral declarations */
#include "Command/command.h"
#include "CLK/CLK.h"
#include "BSP/BSP_if.h"
#include "gpio.h"
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



/***********************************************************************************************
*
* @brief    main() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
int main(void)
{
	 Clk_Init();
#if 0
	CONFIG_PIN_AS_GPIO(PTH,PTH1,OUTPUT); /*  */
    OUTPUT_SET(PTH,PTH1);
#endif	
	 Bsp_If_Init();
#ifdef LED_MODULE
	LED_If_Set_Backlight(10);
#endif
	 while(1)
	 	{
#ifdef  CTP_MODULE
	CTP_If_Task();
#endif
#ifdef LED_MODULE
	LED_If_Task();
//	debug_if_2s_task();
#endif
#ifdef LCD_MODULE
#ifdef DEBUG
//	LCD_If_Task();
#endif
#endif
		}
}
/**********************************************************************************
Chang history:


**********************************************************************************/

