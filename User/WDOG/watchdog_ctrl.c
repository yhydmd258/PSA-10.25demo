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
#ifdef  WATCHDOG_MODULE

#include "derivative.h" 
#include "../Command/command.h"
#include "wdog.h"
#include "watchdog_if.h"
#include "watchdog_ctrl.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/


/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define WDOG_MULT_FRQ   12/10

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static uint8_t wdog_reach_falg=0;;
static uint32_t wdog_time=0;
static uint32_t timer_count=0;

/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void Watchdog_Ctrl_Reach_Flag_Clear(void);
static void Watchdog_Ctrl_Time_Update(uint16_t time);

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/***********************************************************************************************
*
* @brief    Deinit watchdog
* @param    none
* @return   none
*
************************************************************************************************/
void Watchdog_Ctrl_Deinit(void)
{
    WDOG_DeInit();
}

/***********************************************************************************************
*
* @brief    Watchdog_Ctrl_Configure() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void Watchdog_Ctrl_Configure(uint16_t time)
{
    WDOG_ConfigType WDOGConfig = {0};     /* Watchdog configuration structure */
    
    WDOGConfig.sBits.bWaitEnable   = FALSE;  /* Enable WDOG when wait mode */
    WDOGConfig.sBits.bStopEnable   = FALSE;  /* Enable WDOG when stop mode */
    WDOGConfig.sBits.bDbgEnable    = FALSE;  /* Enable WDOG when debug mode */
    WDOGConfig.sBits.bIntEnable    = FALSE;  /* Enable Intrrupt  */
    WDOGConfig.sBits.bUpdateEnable = TRUE;  /* allow to update register after initialization configuration */
    WDOGConfig.sBits.bDisable      = FALSE; /* Enable WDOG */
    WDOGConfig.sBits.bClkSrc       = WDOG_CLK_INTERNAL_1KHZ;
    WDOGConfig.u16TimeOut          = time*WDOG_MULT_FRQ;  /* watch dog total time */
    WDOGConfig.u16WinTime          = 0;

    wdog_time = time;
    timer_count = 0;
    WDOG_Init(&WDOGConfig);
    Watchdog_Ctrl_Reach_Flag_Clear();

    /* set watchdog priority */
    NVIC_SetPriority(WDOG_IRQn, 3);
}

/***********************************************************************************************
*
* @brief    Watchdog_Ctrl_Reconfigure() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void Watchdog_Ctrl_Reconfigure(uint16_t time)
{
    wdog_time = time;
    timer_count = 0;
    Watchdog_Ctrl_Time_Update(time*WDOG_MULT_FRQ);
    Watchdog_Ctrl_Reach_Flag_Clear();
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void Watchdog_Ctrl_Refresh(void)
{
    timer_count = 0;
    Watchdog_Ctrl_Reach_Flag_Clear();
    WDOG_Feed();
//    WDOG_EnableUpdate();
}

/***********************************************************************************************
*
* @brief    wait the watchdog tome up
* @param    none
* @return   none
*
************************************************************************************************/
uint8_t Watchdog_Ctrl_Reach_Flag_Get(void)
{
    return wdog_reach_falg;
}

/***********************************************************************************************
*
* @brief    Watchdog_Ctrl_Timer() - this function will be called every 1ms
* @param    none
* @return   none
*
************************************************************************************************/
void Watchdog_Ctrl_Timer(void)
{
    if(wdog_time>0)
    {
        timer_count++;
        
        if(timer_count > (wdog_time-1))
        {
            wdog_reach_falg=1;
            timer_count=0;
        }
    }
}

/***********************************************************************************************
*
* @brief    update the time
* @param    none
* @return   none
*
************************************************************************************************/
static void Watchdog_Ctrl_Time_Update(uint16_t time)
{
    uint8_t u8Cs2 =  (WDOG->CS2) & 0x03; 

    DISABLEINTERRUPTS;

    WDOG_Unlock();
    
    WDOG->TOVAL8B.TOVALL  = time;
    WDOG->TOVAL8B.TOVALH  = time >> 8;
    WDOG->CS2    = u8Cs2;
    WDOG->CS1    = WDOG_CS1_EN_MASK;
    
    ENABLEINTERRUPTS;
}

/***********************************************************************************************
*
* @brief    clear the flag of watchdpg time up
* @param    none
* @return   none
*
************************************************************************************************/
static void Watchdog_Ctrl_Reach_Flag_Clear(void)
{
    wdog_reach_falg=0;
}

#endif/* WATCHDOG_MODULE */
