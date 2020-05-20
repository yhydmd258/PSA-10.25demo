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
#ifdef	POWER_MANAGEMENT_MODULE
#include "../Command/command.h"
#include "gpio.h"
#include "power_ctrl.h"
#include "power_data.h"
#include "../Timer/timer_if.h"

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
static STANDBY_GPIO_STATUS Standby_Gpio_Status; /*it's used to record status when the module prepare to enter stanby status */

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/***********************************************************************************************
*
* @brief     Initialize the power GPIO
* @param    none
* @return   none
*
************************************************************************************************/
void Power_Data_GPIO_Init(void)
{
    CONFIG_PIN_AS_GPIO(PTH, PTH6, OUTPUT);	 // LCM/BL/DES_POWER_EN
    OUTPUT_SET(PTH, PTH6);
    CONFIG_PIN_AS_GPIO(PTE, PTE6, OUTPUT);	 // 1V2_EN
    OUTPUT_SET(PTE, PTE6);
    /* wait for normal power supply(3.3V)  */
    Timer_If_Delay(TIMER_DELAY_2MS);

    Standby_Gpio_Status = GPIO_DEINIT_START;
}

/***********************************************************************************************
*
* @brief     Initialize the GPIO which used to wake up
* @param    none
* @return   none
*
************************************************************************************************/
void Power_Data_Wakeup_GPIO_Init(void)
{
    /* wake up pin */
    CONFIG_PIN_AS_GPIO(PTD, PTD1, INPUT);
    GPIO_ENABLE_INPUT(PTD, PTD1);
}
/***********************************************************************************************
*
* @brief     read the status of wakeup pin
* @param    none
* @return   0: require the module active
                1: requore the module go to standby
*
************************************************************************************************/
uint8_t Power_Data_Wakeup_Pin_Read(void)
{
    return GPIO_READ_INPUT(PTD, PTD1);
}

/***********************************************************************************************
*
* @brief     read the GPIO status when deinit module
* @param    none
* @return   none
*
************************************************************************************************/
uint32_t Power_Data_Deinit_Status_Read(void)
{
    return Standby_Gpio_Status;
}

/***********************************************************************************************
*
* @brief     Deinitialize the power GPIO
* @param    none
* @return   none
*
************************************************************************************************/
void Power_Data_GPIO_Deinit_Task(void)
{
    switch(Standby_Gpio_Status)
    {
        case GPIO_DEINIT_START:
            OUTPUT_CLEAR(PTB,PTB1);     /* LCD standby */
            OUTPUT_CLEAR(PTB,PTB5);     /* CS */
            OUTPUT_CLEAR(PTC,PTC2);     /* Configure LCD_UPDN(PTC2) as an output */
            OUTPUT_CLEAR(PTC,PTC3);     /* Configure LCD_SHLR(PTC3) as an output */
            /* wait for 70ms, clear next GPIO */
            Timer_If_Set(EN_LCD_STABDBY_LOW_KEEP, TIMER_COUNT_70MS);
            Standby_Gpio_Status = LCD_STABDBY_LOW_KEEP;
            break;
        case LCD_STABDBY_LOW_KEEP:
            if(TRUE == Timer_If_Check(EN_LCD_STABDBY_LOW_KEEP))
            {
                OUTPUT_CLEAR(PTE, PTE5);  /*PDB*/
                OUTPUT_CLEAR(PTE, PTE6);  /* 1V2_EN*/
                OUTPUT_CLEAR(PTH, PTH0);  /* BISTC */
                /* wait for 70ms, clear next GPIO */
                Timer_If_Set(EN_DESER_PDB_LOW_KEEP, TIMER_COUNT_70MS);
                Standby_Gpio_Status = DESER_PDB_LOW_KEEP;
            }
            break;
        case DESER_PDB_LOW_KEEP:
            if(TRUE == Timer_If_Check(EN_DESER_PDB_LOW_KEEP))
            {
                OUTPUT_CLEAR(PTB,PTB0);     /* LCD_RESET(PTB0):*/
                OUTPUT_CLEAR(PTH, PTH6);    /*LCM/BL/DES_POWER_EN*/
                Standby_Gpio_Status = GPIO_DEINIT_END;
            }
            break;
        case GPIO_DEINIT_END:
        default:
            break;
    }
}

/***********************************************************************************************
*
* @brief    the MCU into stop mode, it will be waked up by KBI0_P25
* @param    none
* @return   none
*
************************************************************************************************/
void Power_Data_MCU_Stop(void)
{
    /* enter into deep sleep mode(stop mode) */
    PMC->SPMSC1 &= ~PMC_SPMSC1_LVDSE_MASK;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;	
    __DSB();
    __WFI();
}
#endif /* POWER_MANAGEMENT_MODULE */
