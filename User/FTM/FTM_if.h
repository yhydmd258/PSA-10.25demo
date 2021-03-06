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

#ifndef _FTM_IF_H_
#define _FTM_IF_H_

#include "derivative.h"
#include "FTM.h"

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
extern void FTM_If_Init(void);
extern void FTM_If_Deinit(void);
extern void FTM_If_Set_LED_PWM(uint16_t Params);
extern void FTM_If_Set_Key_PWM(uint16_t params);

#endif	/* _FTM_IF_H_ */

#endif	/* FTM_MODULE */
