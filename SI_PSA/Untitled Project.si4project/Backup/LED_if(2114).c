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
#include "../PIT/pit_if.h"

#ifdef  DESERIALIZE_MODULE
#include "../Deserialize/deserialize_if.h"
#endif 
#ifdef  FTM_MODULE
#include "../FTM/FTM_if.h"
#endif 

#ifdef ADC_MODULE
#include "../ADC/ADC_if.h"
#endif

#ifdef DEBUG
#include "../LCD/LCD_ctrl.h"
#include "../CTP/CTP_if.h"
#include "nvic.h"
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
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_If_Init(void)
{
    LED_Ctrl_Init();
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_If_Standby_Set(UINT8 standby_state)
{
    LED_Ctrl_Standby_Set(standby_state);
}
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_If_Task(void)
{
#ifdef  DESERIALIZE_MODULE
UINT8 tem;
	if(Deserialize_If_Backlight_Flag()==1)
		{
			tem=Deserialize_If_Get_Backlight();
			#ifdef FTM_MODULE
			FTM_If_Set_LED_PWM(tem);
			#else
			LED_If_Set_Backlight(tem);
			#endif			
			Deserialize_If_Backlight_Flag_update(0);
		}
#endif

}

#ifdef DEBUG
void debug_if_2s_task(void)
{
	static UINT32 i=0;
	UINT8 tem[]={0X79,0X6A,0X09,0X00,0X00,0X00,0X00,0X00,0X6A,0X6A};
//	UINT8 tem[]={0X78,0X66,0X00,0X00,0X00,0X01};
	i++;
	if(i==100)  //1000x2ms
		{
		// LCD_Ctrl_Register_Set();
		 CTP_If_Cmd_Send(tem,10);
		 i=0;
		}
	if(i==2000)
		{
		LED_If_Set_Backlight(10);
		}
	if(i==3000)
		{
		
	//	Disable_Interrupt(PIT_CH1_IRQn);
	//	CTP_If_Cmd_Send(tem,5);
	//	Enable_Interrupt(PIT_CH1_IRQn);
	// i=0;
		}
}
#endif
/***********************************************************************************************
*
* @brief    LED_Ctrl_PWM_Callback() - this function is called every 0.05ms, the frequency of LED backlight is 200HZ
* @param    none
* @return   none
*
************************************************************************************************/
void LED_If_PWM_Callback(void)
{
    LED_Ctrl_PWM_Callback();
}
/***********************************************************************************************
*
* @brief    Deserialize_Data_GPIO_INIT - set the GPIO of deserialize
* @param    none
* @return   none
*
************************************************************************************************/  
void LED_If_Set_Backlight(UINT8 value)
{    
    LED_Ctrl_Backlight_Set(value);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - this function is called every 0.05ms, the frequency of LED backlight is 200HZ
* @param    none
* @return   none
*
************************************************************************************************/
void LED_If_Cmd_Send(UINT8* data, UINT8 data_size)
{
#ifdef  DESERIALIZE_MODULE
    Deserialize_If_Cmd_Send(data, data_size);    
#endif
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - this function is called every 0.05ms, the frequency of LED backlight is 200HZ
* @param    none
* @return   none
*
************************************************************************************************/
UINT16 LED_If_Led_Ntc_Adc_Get(void)
{
#ifdef ADC_MODULE
    return ADC_If_Value_Get(ADC_IF_LED_NTC);
#else
    return ADC_LED_NTC_56C;// temp code
#endif
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - this function is called every 0.05ms, the frequency of LED backlight is 200HZ
* @param    none
* @return   none
*
************************************************************************************************/
UINT16 LED_If_Input_Vol_Adc_Get(void)
{
#ifdef ADC_MODULE
    return ADC_If_Value_Get(ADC_IF_INPUT_VOL);
#else
    return ADC_INPUT_VOLTAGE_12V;// temp code
#endif
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - this function is called every 0.05ms, the frequency of LED backlight is 200HZ
* @param    none
* @return   none
*
************************************************************************************************/
UINT16 LED_If_Pcb_Ntc_Adc_Get(void)
{
#ifdef ADC_MODULE
    return ADC_If_Value_Get(ADC_IF_PCB_NTC);
#else
    return ADC_PCB_NTC_57C;// temp code
#endif
}

#endif /* LED_MODULE */
