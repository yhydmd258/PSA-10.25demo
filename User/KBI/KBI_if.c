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

#include <stdio.h>
#include "derivative.h" 
#include "KBI_if.h"
#include "kbi.h"
#ifdef  CTP_MODULE
#include "../CTP/CTP_if.h"
#endif  
#include "gpio.h"

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
typedef void (*SKBI_FUNCTION)(void);

static SKBI_FUNCTION sKBI1_Function_Table[KBI_MAX_PINS_PER_PORT]=
{
    NULL,   /* KBI1_P0 */
    NULL,   /* KBI1_P1 */
    NULL,   /* KBI1_P2 */
    NULL,   /* KBI1_P3 */
    NULL,   /* KBI1_P4 */
    NULL,   /* KBI1_P5 */
    NULL,   /* KBI1_P6 */
    NULL,   /* KBI1_P7 */
    NULL,   /* KBI1_P8 */
    NULL,   /* KBI1_P9 */
    NULL,   /* KBI1_P10 */
    NULL,   /* KBI1_P11 */
    NULL,   /* KBI1_P12 */
    NULL,   /* KBI1_P13 */
    NULL,   /* KBI1_P14 */
    NULL,   /* KBI1_P15 */
#ifdef  CTP_MODULE
    CTP_If_Touch_Interrupt_Notice,   /* KBI1_P16 */
#else
    NULL,   /* KBI1_P16 */
#endif
    NULL,   /* KBI1_P17 */
    NULL,   /* KBI1_P18 */
    NULL,   /* KBI1_P19 */
    NULL,   /* KBI1_P20 */
    NULL,   /* KBI1_P21 */
    NULL,   /* KBI1_P22 */
    NULL,   /* KBI1_P23 */
    NULL,   /* KBI1_P24 */
    NULL,   /* KBI1_P25 */
    NULL,   /* KBI1_P26 */
    NULL,   /* KBI1_P27 */
    NULL,   /* KBI1_P28 */
    NULL,   /* KBI1_P29 */
    NULL,   /* KBI1_P30 */
    NULL    /* KBI1_P31 */
};
/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/
#define KBI_1 KBI1
#define KBI_0 KBI0
/**********************************************************************************************
* Local variables
**********************************************************************************************/


/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void KBI1_Handler(void);

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/***********************************************************************************************
*
* @brief    init the KBI1 module
* @param    none
* @return   none
*
************************************************************************************************/
void KBI_If_Init(void)
{
    uint32_t          i;
    KBI_ConfigType  KBI1_Config={{0}};

    /* Disable all the KBI pins */
    for (i = 0; i < KBI_MAX_PINS_PER_PORT; i++)
    {
        KBI1_Config.sPin[i].bEn	 = 0;
    }

    KBI1_Config.sBits.bRstKbsp   = 1;			/* Writing a 1 to RSTKBSP is to clear the KBIxSP Register*/
    KBI1_Config.sBits.bKbspEn   	= 1;		/* The latched value in KBxSP register while interrupt flag occur to be read.*/
    KBI1_Config.sBits.bMode   	= KBI_MODE_EDGE_ONLY;
    KBI1_Config.sBits.bIntEn  	= 1;

    /*Falling edge/low level select; KBI1_P16(CTP) channel enable */
    KBI1_Config.sPin[16].bEdge    = KBI_FALLING_EDGE_LOW_LEVEL;
    KBI1_Config.sPin[16].bEn      = 1;

    KBI_SetCallback(KBI_1, &KBI1_Handler);
    KBI_Init(KBI_1, &KBI1_Config);

    /* set DESERIALIZER_I2C priority */
    NVIC_SetPriority(KBI1_IRQn, 3);
}

/***********************************************************************************************
*
* @brief    KBI_0 is used to wake up in stop mode
* @param    none
* @return   none
*
************************************************************************************************/
void KBI_If_Wakeup_Init(void)
{
    uint32_t          i;
    KBI_ConfigType  KBI0_Config={{0}};

    /* Disable all the KBI pins */
    for (i = 0; i < KBI_MAX_PINS_PER_PORT; i++)
    {
        KBI0_Config.sPin[i].bEn  = 0;
    }

    KBI0_Config.sBits.bRstKbsp   = 1;           /* Writing a 1 to RSTKBSP is to clear the KBIxSP Register*/
    KBI0_Config.sBits.bKbspEn       = 1;        /* The latched value in KBxSP register while interrupt flag occur to be read.*/
    KBI0_Config.sBits.bMode     = KBI_MODE_EDGE_ONLY;
    KBI0_Config.sBits.bIntEn    = 1;

    /*Rising edge/high level select; KBI0_P25(ACC_Wake) channel enable */
    KBI0_Config.sPin[25].bEdge    = KBI_FALLING_EDGE_LOW_LEVEL;
    KBI0_Config.sPin[25].bEn      = 1;

//    KBI_SetCallback(KBI_0, &KBI0_Handler);
    KBI_Init(KBI_0, &KBI0_Config);
}

/***********************************************************************************************
*
* @brief    Deinit the KBI_1 module
* @param    none
* @return   none
*
************************************************************************************************/
void KBI_If_Deinit(void)
{
    KBI_SetCallback(KBI_1, NULL);
    KBI_DeInit(KBI_1);
}

/***********************************************************************************************
*
* @brief    the interrupt service function
* @param    none
* @return   none
*
************************************************************************************************/
static void KBI1_Handler(void)
{
    uint32_t  i=0;
    uint32_t sKBI1_SP=0;

    sKBI1_SP = KBI_GetSP(KBI_1);    

    for(i=0; i< KBI_MAX_PINS_PER_PORT; i++)
    {
        if((sKBI1_SP&(0x01<<i)) && sKBI1_Function_Table[i])
        {
            sKBI1_Function_Table[i]();
        }
    }

    KBI_RstSP(KBI_1);
}

#endif /* KBI_MODULE */
