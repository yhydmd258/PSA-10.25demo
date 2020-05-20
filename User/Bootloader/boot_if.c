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

#ifdef PIT_MODULE
#include "../PIT/pit_if.h"
#endif
#include "../Command/command.h"

#include "boot_if.h"
#include "boot_ctrl.h"

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
static void Boot_If_Switch_To_App(void);

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/***********************************************************************************************
*
* @brief    initialize the boot module
* @param    none
* @return   none
*
************************************************************************************************/
void Boot_If_Init(void)
{
#ifdef PIT_MODULE
    PIT_If_Init();
#endif

    Boot_Ctrl_Init();
}

/***********************************************************************************************
*
* @brief    the task function
* @param    none
* @return   none
*
************************************************************************************************/
void Boot_If_Upgrade_Task(void)
{
    /* read command from the buf which used in I2C ISR function*/
    Boot_Ctrl_Cmd_Read();
    
    /* execute the upgrade */
    if(TRUE == Boot_Ctrl_Upgrade_Result_Check())
        Boot_Ctrl_Upgrade_Status_Manage();
    else
        Boot_If_Switch_To_App();

    /* request to send data */
    Boot_Ctrl_Cmd_Send_Check();
}

/***********************************************************************************************
*
* @brief     switch to appliction mode
* @param    none
* @return   none
*
************************************************************************************************/
static void Boot_If_Switch_To_App(void)
{
    uint32_t app_entry = 0;
    uint32_t app_stack = 0;
    void (*jump_to_appliction)(void);

#ifdef PIT_MODULE
    PIT_If_Deinit();
#endif

    Boot_Ctrl_Deinit();

    app_entry = *(uint32_t*)(APP_IMAGE_START_ADDR+4);
    app_stack = *(uint32_t*)APP_IMAGE_START_ADDR;
    
    jump_to_appliction = (void(*)(void))app_entry ;

    /* set Vector Table Offset Register*/
    SCB->VTOR = APP_IMAGE_START_ADDR;

    __set_MSP(app_stack);
    __set_PSP(app_stack);

    jump_to_appliction();
}

#endif /* BOOTLOADER */
