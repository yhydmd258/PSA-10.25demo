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
#ifdef  KBI_MODULE

#ifndef _KBI_IF_H_
#define _KBI_IF_H_
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
extern void KBI_If_Init(void);
extern void KBI_If_Wakeup_Init(void);
extern void KBI_If_Deinit(void);

#ifdef __cplusplus
}
#endif

#endif

#endif/* KBI_MODULE */
