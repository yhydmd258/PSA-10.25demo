/******************************************************************************								
*  Name: Main.c
*  Description: 
*  Project: C_demo
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/		
#include "CLK.h"
#include "../Headers/ics.h"

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
* @brief    CLK_Init - Initialize the clocks to run at 20 MHz from the IRC
* @param    none
* @return   none
*
************************************************************************************************/  
void Clk_Init()

{
#if 0
    /* FEI */
    ICS_C2|=ICS_C2_BDIV(1)  ;           /*BDIV=2, use default until clock dividers configured */
    ICS_C1 =ICS_C1_CLKS(0x00) |         /* choose FLL as the clock source */
            ICS_C1_IREFS_MASK  |            /* Reference clock frequency = 37.5 KHz*/
            ICS_C1_IRCLKEN_MASK;            /* Enable the internal reference clock*/  
    while(!(ICS_S & ICS_S_LOCK_MASK));   /* Wait for FLL lock, now running at 48 MHz (1280 * 37.5Khz) */       
    SIM->CLKDIV = SIM_CLKDIV_OUTDIV1(0x00) |    /* select ICSOUTCLK as core clock */
                    SIM_CLKDIV_OUTDIV2_MASK |       /* bus clock = core clock/2 */
                    SIM_CLKDIV_OUTDIV3_MASK;        /* timing clock = core clock/2 */
    ICS_C2|=ICS_C2_BDIV(0)  ;           /*BDIV=0, Bus clock = 24 MHz*/
    ICS_S |= ICS_S_LOCK_MASK ;          /* Clear Loss of lock sticky bit */ 
#endif

#if 0
    ICS_C1|=ICS_C1_IRCLKEN_MASK;        /* Enable the internal reference clock*/ 
    ICS_C3= 0x90;                       /* Reference clock frequency = 31.25 KHz*/      
    while(!(ICS_S & ICS_S_LOCK_MASK));   /* Wait for PLL lock, now running at 40 MHz (1280 * 31.25Khz) */       
    ICS_C2|=ICS_C2_BDIV(1)  ;           /*BDIV=2, Bus clock = 20 MHz*/
    ICS_S |= ICS_S_LOCK_MASK ;          /* Clear Loss of lock sticky bit */ 
#endif

#if 1 /* external clock/FEE, core clock is 40MHz = 8M/256*1280, bus clock is 20MHz*/
    OSC_CR = 0x96; /* high-range, high-gain oscillator selected */
    while(0 == (OSC_CR & OSC_CR_OSCINIT_MASK)); /* wait until oscillator is ready */
    ICS_C2 = 0x20;  /* BDIV=divide by 2 - use default until clock dividers configured */
    ICS_C1 = 0x18;  /* 8MHz external reference clock/256 as source to FLL */
    while(1 == (ICS_S & ICS_S_IREFST_MASK)); /* wait for external source selected */
    while(0 == (ICS_S & ICS_S_LOCK_MASK)); /* wait for FLL to lock */
    SIM_CLKDIV = 0x01000000; /* core clock = ICSOUT/1 and bus clock = core clock/2 */
    ICS_C2 = 0x00;  /* BDIV=divide by 1 - allows max core and bus clock frequencies */
    
#else

    /* Perform processor initialization */
    ICS_ConfigType ICS_set={0};             /* Declaration of ICS_setup structure */

    ICS_set.u8ClkMode=ICS_CLK_MODE_FEE;
    ICS_set.bdiv=0;
    ICS_set.oscConfig.bGain = TRUE;
    ICS_set.oscConfig.bRange = TRUE;
    ICS_set.oscConfig.bIsCryst = TRUE;
    ICS_set.oscConfig.bEnable = TRUE;
    ICS_set.oscConfig.bWaitInit = TRUE;
    ICS_set.oscConfig.u32OscFreq = 8000;/* external oscillator is 8M, external clock frequency 8M/256= 31.25K */
    
    ICS_Init(&ICS_set);                     /* Initialization of Core clock at 48 MHz, Bus clock at 24 MHz*/
#endif
}
