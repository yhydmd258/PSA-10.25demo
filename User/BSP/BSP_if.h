/******************************************************************************								
*  Name: .c
*  Description: 
*  Project: C_demo
*  Auther:  shanghai tianma
*              R&D center
*              yhy
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/		
#ifndef _BSP_IF_H_
#define _BSP_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "project_cfg.h" 

#include "derivative.h"

#ifdef  KBI_MODULE
#include "../KBI/KBI_if.h"
#endif

#ifdef  DESERIALIZE_MODULE
#include "../Deserialize/deserialize_if.h"
#endif  

#ifdef  CTP_MODULE
#include "../CTP/CTP_if.h"
#endif  

#ifdef  LCD_MODULE
#include "../LCD/LCD_if.h"
#endif  

#ifdef FTM_MODULE
#include "../FTM/FTM_if.h"
#endif

#ifdef KEY_MODULE
#include "../KEY/key_if.h"
#endif

#ifdef LED_MODULE
#include "../LED/LED_if.h"
#endif

#ifdef DIAGNOSIS_MODULE
#include "../Diagnosis/diagnosis_if.h"
#endif

#ifdef EEPROM_MODULE
#include "../EEPROM/EEPROM_if.h"
#endif

#ifdef PIT_MODULE
#include "../PIT/pit_if.h"
#endif

#ifdef ADC_MODULE
#include "../ADC/ADC_if.h"
#endif

#ifdef WATCHDOG_MODULE
#include "../WDOG/watchdog_if.h"
#endif

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
* Global functions
**********************************************************************************************/
extern void Bsp_If_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* _BSP_IF_H_ */

