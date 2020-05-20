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
#ifdef FTM_MODULE

#include "FTM_ctrl.h"
#include "FTM_if.h"

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

/***********************************************************************************************
*
* @brief    init the FTM module
* @param    none
* @return   none
*
************************************************************************************************/
void FTM_If_Init(void)
{
    FTM_Ctrl_Init();
}

/***********************************************************************************************
*
* @brief    Denit the FTM module
* @param    none
* @return   none
*
************************************************************************************************/
void FTM_If_Deinit(void)
{
    FTM_Ctrl_Deinit();
}

/***********************************************************************************************
*
* @brief
* @param    "params" is percent
* @return   none
*
************************************************************************************************/
void FTM_If_Set_LED_PWM(uint16_t params)
{
    FTM_Ctrl_Set_LED_PWM(params);
}

/***********************************************************************************************
*
* @brief
* @param    "params" is percent
* @return   none
*
************************************************************************************************/
void FTM_If_Set_Key_PWM(uint16_t params)
{
    FTM_Ctrl_Set_Key_PWM(params);
}

#endif	/* FTM_MODULE */
