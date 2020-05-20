#include "project_cfg.h" 
#ifdef LED_MODULE
#ifndef _LED_IF_H_
#define _LED_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

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
/* input voltage, theoretical value of ADC = input voltage*(22K/(150K+22K))/(3.3V/2^12)= 158.76*input voltage */
#define ADC_INPUT_VOLTAGE_7V    1050
#define ADC_INPUT_VOLTAGE_8V    1208
#define ADC_INPUT_VOLTAGE_12V   1867
#define ADC_INPUT_VOLTAGE_17V   2700
#define ADC_INPUT_VOLTAGE_19V   3011

/*PCB NTC, theoretical value of ADC = resistance/(resistance+10K)*3.3V/(3.3V/2^12)=4096/(1+10K/resistance)*/
#define ADC_PCB_NTC_109C        300  /* environment temperature is 85C, the resistance is 777*/
#define ADC_PCB_NTC_96C         400  /*environment temperature is 65C, the resistance is 1081*/
#define ADC_PCB_NTC_57C         1020 /*environment temperature is 25C, the resistance is 3315*/
#define ADC_PCB_NTC_N_11C       2591 /*environment temperature is -40C, the resistance is 17214*/

/*LED NTC, theoretical value of ADC = resistance/(resistance+10K)*3.3V/(3.3V/2^12)=4096/(1+10K/resistance)*/
#define ADC_LED_NTC_104C        0/* environment temperature is 85C, the resistance is */
#define ADC_LED_NTC_95C         0/*environment temperature is 65C, the resistance is */
#define ADC_LED_NTC_56C         0/*environment temperature is 25C, the resistance is */
#define ADC_LED_NTC_N_12C       0/*environment temperature is -40C, the resistance is */

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
extern void LED_If_Init(void);
extern void LED_If_Standby_Set(UINT8 standby_state);
extern void LED_If_Task(void);
extern void LED_If_PWM_Callback(void);
extern void LED_If_Set_Backlight(UINT8 value);
extern void LED_If_Cmd_Send(UINT8* data, UINT8 data_size);
extern UINT16 LED_If_Led_Ntc_Adc_Get(void);
extern UINT16 LED_If_Input_Vol_Adc_Get(void);
extern UINT16 LED_If_Pcb_Ntc_Adc_Get(void);
#ifdef DEBUG
extern void debug_if_2s_task(void);
#endif


#ifdef __cplusplus
}
#endif

#endif /* _LED_IF_H_ */
#endif /* LED_MODULE */
