#include "project_cfg.h" 
#ifdef LED_MODULE
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
#include "LED_if.h"
#include "LED_ctrl.h"
#include "LED_data.h"
#include "../Command/command.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/


/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define PWM_PERCENT_MAX     100

/**********************************************************************************************
* Local types
**********************************************************************************************/
static UINT8 LED_Ctrl_PWM_Percent=100;
static UINT8 LED_Ctrl_PWM_Percent_Last=100;

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/
//static UINT8 LED_Ctrl_Backlight_Get(void);

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
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Ctrl_Init(void)
{
    LED_Data_I2C_Init();
    LED_Data_Cfg_Init();
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Ctrl_Standby_Set(UINT8 standby_state)
{
    if(STANDBY_ON == standby_state)
        LED_Data_Power_Set(0);/* power off */
    else
        LED_Data_Power_Set(1);/* power on */
}

/***********************************************************************************************
*
* @brief    LED_Ctrl_PWM_Callback() - this function is called every 0.05ms, the frequency of LED backlight is 200HZ
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Ctrl_PWM_Callback(void)
{

}

/***********************************************************************************************
*
* @brief    Deserialize_Data_GPIO_INIT - set the GPIO of deserialize
* @param    none
* @return   none
*
************************************************************************************************/  
void LED_Ctrl_Backlight_Set(UINT8 value)
{
	LED_Data_Set_PWM(value);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Ctrl_Fault_Detect(void)
{
    UINT16 fault_msg=0;
    static UINT8 read_fault_flag=0;
    /* if error */

    if(0==LED_Data_Fault_Status_Read())
    {
        if(0==read_fault_flag)
        {
            fault_msg = LED_Data_Fault_Msg_Read();
            LED_Data_Fault_Cmd_Make(fault_msg);
            read_fault_flag = 1;
        }
        LED_Data_Fault_Msg_Clear();
    }
    
    else
    {
        if(1 == read_fault_flag)
        {
            fault_msg = LED_Data_Fault_Msg_Read();
            LED_Data_Fault_Cmd_Make(fault_msg);
            read_fault_flag=0;
        }
    }

}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Ctrl_Backlight_Detect(void)
{
    UINT16  adc_vol_value;
//    UINT16  adc_led_value;
    UINT16  adc_pcb_value;
//    UINT16  adc_led_value;
//    INT8    temp_adc_led_pwm;

    /* Input Vol function*/
    adc_vol_value = LED_If_Input_Vol_Adc_Get();
        /*1. LED backlight is bright when voltage is rise from 8V to 19V;
                2. LED backlight is bright when voltage is fail from 17V to 7v; 
                3. LED backlight is dark in other case */
        /* power on */
    if(LED_Data_Power_En_Check())
    {
        if((adc_vol_value<ADC_INPUT_VOLTAGE_7V) || (adc_vol_value>ADC_INPUT_VOLTAGE_19V))
            LED_Data_Power_Set(0);/* power off */
    }
    else
    {
        if((adc_vol_value>ADC_INPUT_VOLTAGE_8V) && (adc_vol_value<ADC_INPUT_VOLTAGE_17V))
            LED_Data_Power_Set(1);/* power on */
    }

    /* PCB NTC */
    adc_pcb_value = LED_If_Pcb_Ntc_Adc_Get();
    /*backlight start derating when the environment temperature is 65C,  PWM is 50% when 85C,
          backlight is turn off when the environment temperature is 85C */
//    if((1==LED_Data_Fault_Status_Read()) && LED_Data_Power_En_Check())
//    if(LED_Data_Power_En_Check())
    {
//        if(LED_Ctrl_PWM_Percent_Last>0)
        {
            /* environment temperature>85C, environment temperature<-40C */
            if((adc_pcb_value<=ADC_PCB_NTC_109C) || (adc_pcb_value>=ADC_PCB_NTC_N_11C))
            {
                LED_Ctrl_PWM_Percent=0;
            }
            /*backlight start derating when the environment temperature is 65C,  PWM is 50% when 85C*/
            else if((adc_pcb_value<=ADC_PCB_NTC_96C)
                &&(adc_pcb_value>ADC_PCB_NTC_109C))
            {
                LED_Ctrl_PWM_Percent = adc_pcb_value/2-100;
            }
            else
            {
                LED_Ctrl_Backlight_Set(LED_Ctrl_PWM_Percent_Last);
            }
        }
    }

    /* LED NTC */
}
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Ctrl_Cmd_Send(UINT8* data, UINT8 data_size)
{
    LED_If_Cmd_Send(data, data_size);
}

/***********************************************************************************************
*
* @brief    Deserialize_Data_GPIO_INIT - set the GPIO of deserialize
* @param    none
* @return   none
*
************************************************************************************************/ 
#if 0
static UINT8 LED_Ctrl_Backlight_Get(void)
{
    return LED_Ctrl_PWM_Percent;
}
#endif
#endif /* LED_MODULE */
