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
#ifdef PIT_MODULE

#include <stdio.h>
#include "../Command/command.h"
#include "../Timer/timer_if.h"
#include "pit_if.h"
#include "pit.h"

#ifdef LED_MODULE
#include "../LED/LED_if.h"
#endif
#ifdef  CTP_MODULE
#include "../CTP/CTP_if.h"
#endif  
#ifdef  DESERIALIZE_MODULE
#include "../Deserialize/deserialize_if.h"
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
typedef void (*PIT_If_Callback)(void);             /*!< PIT callback type */

/**********************************************************************************************
* Local types
**********************************************************************************************/


/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/
static void PIT_If_Task0(void);
static void PIT_If_Task1(void);

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static PIT_If_Callback Pit_If_Channel0_Callback[]=
    {
#ifdef LED_MODULE
      //  LED_If_PWM_Callback,
#endif
        NULL
    };
static PIT_If_Callback Pit_If_Channel1_Callback[]=
    {
        &Timer_If_Count,
			
#ifdef  CTP_MODULE
        //&CTP_If_2ms_Task,        
#endif
        NULL
    };

#define CHANNEL0_FUNCTION_NUMBER    sizeof(Pit_If_Channel0_Callback)/sizeof(PIT_If_Callback)
#define CHANNEL1_FUNCTION_NUMBER    sizeof(Pit_If_Channel1_Callback)/sizeof(PIT_If_Callback)
	
/**********************************************************************************************
* Local functions
**********************************************************************************************/


/**********************************************************************************************
* Global functions
**********************************************************************************************/
/***********************************************************************************************
*
* @brief    Init PIT module
* @param    none
* @return   none
*
************************************************************************************************/
void PIT_If_Init(void)
{
    PIT_ConfigType  PIT_Config ={0};
    UINT32        u32LoadValue;
    
    PIT_ConfigType  PIT_Config1 ={0};
    UINT32        u32LoadValue1;
    /* configure PIT module in chain mode */
    /* PIT clock source is bus clock,20MHz */
    /* generate an interrupt every 0.05ms, PIT channel 0 load value = 0.05ms/(1/20000000s)-1=1000-1=0x3E7,*/
    u32LoadValue   = 0x3E7;               /* PIT CH0 timer load value */
//    u32LoadValue   = 0x1E8480; /*100ms*/
    
    /* configure PIT channel 0, only enable timer */
    PIT_Config.u32LoadValue      = u32LoadValue;
    PIT_Config.bFreeze           = FALSE;
    PIT_Config.bModuleDis        = FALSE;    	/* Enable PIT module */
    PIT_Config.bInterruptEn      = TRUE;
    PIT_Config.bChainMode        = FALSE;      /* disable time link */
    PIT_Config.bTimerEn          = TRUE;

    PIT_SetCallback(PIT_CHANNEL0, PIT_If_Task0);
    PIT_Init(PIT_CHANNEL0, &PIT_Config);
    NVIC_SetPriority(PIT_CH0_IRQn, 2);
		
    /* configure PIT module in chain mode */
    /* PIT clock source is bus clock,20MHz */
    /* generate an interrupt every 2ms, PIT channel 1 load value = 2ms/(1/20000000s)-1=40000-1=0x9C3F,*/
    u32LoadValue1   = 0x9C3F;               /* PIT CH1 timer load value */
    
    /* configure PIT channel 0, only enable timer */
    PIT_Config1.u32LoadValue      = u32LoadValue1;
    PIT_Config1.bFreeze           = FALSE;
    PIT_Config1.bModuleDis        = FALSE;    	/* Enable PIT module */
    PIT_Config1.bInterruptEn      = TRUE;
    PIT_Config1.bChainMode        = FALSE;      /* disable time link */
    PIT_Config1.bTimerEn          = TRUE;

    PIT_SetCallback(PIT_CHANNEL1, PIT_If_Task1);
    PIT_Init(PIT_CHANNEL1, &PIT_Config1);
	NVIC_SetPriority(PIT_CH1_IRQn, 1);
    
}

/***********************************************************************************************
*
* @brief    Deinit PIT module
* @param    none
* @return   none
*
************************************************************************************************/
void PIT_If_Deinit(void)
{
    PIT_SetCallback(PIT_CHANNEL1, NULL);
	PIT_SetCallback(PIT_CHANNEL0, NULL);
    PIT_DeInit();
}
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
    void PIT_Task0(void)
************************************************************************************************/
static void PIT_If_Task0(void)
{
    UINT8 i;

    for(i=0; i<CHANNEL0_FUNCTION_NUMBER; i++)
    {
        if(Pit_If_Channel0_Callback[i])
            Pit_If_Channel0_Callback[i]();
    }
}
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
    void PIT_Task0(void)
************************************************************************************************/
static void PIT_If_Task1(void)
{
    UINT8 i;

    for(i=0; i<CHANNEL1_FUNCTION_NUMBER; i++)
    {
        if(Pit_If_Channel1_Callback[i])
            Pit_If_Channel1_Callback[i]();
    }
}
#endif
