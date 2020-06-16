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

#include "../Command/command.h"
#include "FTM_ctrl.h"
#include "FTM_if.h"
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
#define FTM2_MODULE_VALUE              12499  /*	200 Hz/ 5 ms duty cycle PWM */
#define FTM2_CHANNEL0_CNV_DEFAULT      12500
#define FTM2_CHANNEL0_PWM_MULTIPLE     125     /* It's percent; 12500/100=125  */

#define FTM2_CHANNEL1_CNV_DEFAULT      12500
#define FTM2_CHANNEL1_PWM_MULTIPLE     125     /* It's percent; 12500/100=125  */

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static uint8_t FTM_INIT_FLAG = FALSE;
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
void FTM_Ctrl_Init(void)
{
    FTM_ConfigType FTM2_Config = {0};
//    FTM_ChParamsType FTM2CH0_Config = {0};
    FTM_ChParamsType FTM2CH1_Config = {0};

    FTM2_Config.modulo = FTM2_MODULE_VALUE;	//	200 Hz/ 5 ms duty cycle PWM
    FTM2_Config.clk_source = FTM_CLOCK_SYSTEMCLOCK;	//	40 MHz clock source
    FTM2_Config.prescaler = FTM_CLOCK_PS_DIV16;	//	40 MHz / 16 = 2.5 MHz
    FTM2_Config.mode = 1;
    FTM2_Config.toie = 0;
#if 0
    SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS0(2); /* Select Pins corresponds to the PTF0 for output */
    FTM2CH0_Config.ctrl.bits.bMode = FTM_PWMMODE_EDGEALLIGNED;
    FTM2CH0_Config.ctrl.bits.bPWMPol = FTM_PWM_HIGHTRUEPULSE;
    FTM2CH0_Config.u16CnV = FTM2_CHANNEL0_CNV_DEFAULT;	// initial value
    FTM_ChannelInit(FTM2, FTM_CHANNEL_CHANNEL0, FTM2CH0_Config);
#endif
    SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS1(1); /* Select Pins corresponds to the PTH1 for output */
    FTM2CH1_Config.ctrl.bits.bMode = FTM_PWMMODE_EDGEALLIGNED;
    FTM2CH1_Config.ctrl.bits.bPWMPol = FTM_PWM_HIGHTRUEPULSE;
    FTM2CH1_Config.u16CnV = FTM2_CHANNEL1_CNV_DEFAULT;	// initial value
    FTM_ChannelInit(FTM2, FTM_CHANNEL_CHANNEL1, FTM2CH1_Config);

    FTM_Init(FTM2, &FTM2_Config);
//	FTM_SetChannelValue(FTM2, FTM_CHANNEL_CHANNEL1, 80*FTM2_CHANNEL1_PWM_MULTIPLE);
	FTM_SetChannelValue(FTM2, FTM_CHANNEL_CHANNEL1, 60*FTM2_CHANNEL1_PWM_MULTIPLE);

    FTM_INIT_FLAG = TRUE;
}

/***********************************************************************************************
*
* @brief    Deinit the FTM module
* @param    none
* @return   none
*
************************************************************************************************/
void FTM_Ctrl_Deinit(void)
{
    FTM_DeInit(FTM2);
    
    FTM_INIT_FLAG = FALSE;

//    CONFIG_PIN_AS_GPIO(PTE, PTF0, INPUT);
//    OUTPUT_CLEAR(PTE, PTF0);                  
//    CONFIG_PIN_AS_GPIO(PTE, PTF1, INPUT);
//    OUTPUT_CLEAR(PTE, PTF1);                  
}

/***********************************************************************************************
*
* @brief     set backlight pwm by the pin "FTM2_CH0"
* @param    "params" is percent
* @return   none
*
************************************************************************************************/
void FTM_Ctrl_Set_LED_PWM(uint16_t params)
{
    if(TRUE == FTM_INIT_FLAG)
        FTM_SetChannelValue(FTM2, FTM_CHANNEL_CHANNEL1, params*FTM2_CHANNEL1_PWM_MULTIPLE);
}

/***********************************************************************************************
*
* @brief     set backlight pwm by the pin "FTM2_CH1"
* @param    "params" is percent
* @return   none
*
************************************************************************************************/
void FTM_Ctrl_Set_Key_PWM(uint16_t params)
{
    if(TRUE == FTM_INIT_FLAG)
        FTM_SetChannelValue(FTM2, FTM_CHANNEL_CHANNEL1, params*FTM2_CHANNEL1_PWM_MULTIPLE);
}

#endif	/* FTM_MODULE */
