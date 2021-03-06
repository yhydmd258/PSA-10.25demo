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
#ifdef ADC_MODULE
#include "ADC_data.h"
#include "ADC_ctrl.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/


/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define ADC_VOLTAGE_BASE    0x32  /* 3.3V/4096(12bit)*0x32 = 0.04V*/
#define ADC_VOLTAGE_UNIT    (3.3/4096)

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
* @brief    init ADC module
* @param    none
* @return   none
*
************************************************************************************************/
void ADC_Ctrl_Init(void)
{
    ADC_Data_Init();
}

/***********************************************************************************************
*
* @brief    deinit ADC module
* @param    none
* @return   none
*
************************************************************************************************/
void ADC_Ctrl_Deinit(void)
{
    ADC_Data_Deinit();
}
/***********************************************************************************************
*
* @brief    get ADC value according to the type
* @param    adc_type:  input voltage, PCB NTC, backlight NTC
* @return   uint16_t: the value of ADC
*
************************************************************************************************/
uint16_t ADC_Ctrl_Value_Detect(ADC_IF_TYPE adc_type)
{
    switch(adc_type)
    {
        case ADC_IF_LED_NTC:
            return ADC_Data_LED_NTC_Get();
        case ADC_IF_INPUT_VOL:
            return ADC_Data_Input_Vol_Get();
        case ADC_IF_PCB_NTC:
        default:
            return ADC_Data_PCB_NTC_Get();
    }
}

#endif /* ADC_MODULE */
