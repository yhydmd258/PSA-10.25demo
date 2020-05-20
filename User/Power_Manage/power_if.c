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
#ifdef POWER_MANAGEMENT_MODULE
#include "../BSP/BSP_if.h"
#include "power_if.h"
#include "power_ctrl.h"

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
* @brief     Initialize the power manage module
* @param    none
* @return   none
*
************************************************************************************************/
void Power_If_Init(void)
{
    Power_Ctrl_Init();
}

/***********************************************************************************************
*
* @brief     Deinit the power manage module after exit from the standby mode
* @param    none
* @return   none
*
************************************************************************************************/
void Power_If_Deinit(void)
{
    Power_Ctrl_Deinit();
}

/***********************************************************************************************
*
* @brief     This function contain the task, it will be called by the cycle task.
* @param    none
* @return   none
*
************************************************************************************************/
void Power_If_Task(void)
{
    Power_Ctrl_Wakeup_Manage();
}

/***********************************************************************************************
*
* @brief     This function contain the task, it will be called by the cycle task.
* @param    none
* @return   none
*
************************************************************************************************/
uint32_t Power_If_Wakeup_Status_read(void)
{
    return Power_Ctrl_Wakeup_Status_Read();
}
/***********************************************************************************************
*
* @brief     This function contain the task, it will be called by the cycle task.
* @param    none
* @return   none
*
************************************************************************************************/
uint16_t Power_If_Voltage_ADC(void)
{
#ifdef	ADC_MODULE
        return ADC_If_Value_Get(ADC_IF_INPUT_VOL);
#else
        return POWER_INPUT_ADC_12V;
#endif
}
/***********************************************************************************************
*
* @brief     analyse the command
* @param    cmd_buf: the pointer of command
* @return   none
*
************************************************************************************************/
void Power_If_Cmd_Analyse(uint8_t *cmd_buf)
{
    Power_Ctrl_Cmd_Analyse(cmd_buf);
}

/***********************************************************************************************
*
* @brief    save the command data which will be sent
* @param    data: the data need to be made into standard frame
                data_size:  the size of the data need to be made into standard frame
* @return   none
*
************************************************************************************************/
void Power_If_Startup_Cmd_Send(uint8_t* data, uint8_t data_size)
{
#ifdef  DESERIALIZE_MODULE
    Deserialize_If_Startup_Cmd_Send(data, data_size, 0);
#endif
}

/***********************************************************************************************
*
* @brief     read the state of the startup command communication
* @param    void
* @return   TRUE: the state of startup communication machine is OK
                FALSE:  the state of startup communication machine is going or flase.
*
************************************************************************************************/
uint8_t Power_If_Startup_Status_Read(void)
{
    return Power_Ctrl_Startup_Status_Read();
}

/***********************************************************************************************
*
* @brief     init the GPIO which used to wake up before enter into the stop mode
* @param    void
* @return   void
*
************************************************************************************************/
void Power_If_Wakeup_KBI_Init(void)
{
#ifdef  KBI_MODULE
    KBI_If_Wakeup_Init();
#endif
}
/***********************************************************************************************
*
* @brief     from not operation level to full operational level, we will turn on backlight, turn on key backlight,
                turn on CTP, turn on LCD display
* @param    void
* @return   void
*
************************************************************************************************/
void Power_If_Not_Oper_To_Full_Oper(void)
{
#ifdef FTM_MODULE
    FTM_If_Init();
#endif
#ifdef	LED_MODULE
    LED_If_Init();
#endif
#ifdef KEY_MODULE
    Key_If_Init();
#endif
}

/***********************************************************************************************
*
* @brief     from abnormal to full operational level, we will turn on deserialize, turn on key,
                turn on LCD display, turn on CTP,  turn on deserializer.
* @param    void
* @return   void
*
************************************************************************************************/
void Power_If_Abnormal_To_Full_Oper(void)
{
#ifdef POWER_MANAGEMENT_MODULE
    Power_If_Init();
#endif

#ifdef CTP_MODULE
    CTP_If_Init();
#endif

#ifdef  KBI_MODULE
    KBI_If_Init();
#endif

#ifdef DESERIALIZE_MODULE
    Deserialize_If_Init();
#endif

#ifdef LCD_MODULE
    LCD_If_Init();
#endif

#ifdef FTM_MODULE
    FTM_If_Init();
#endif

#ifdef ADC_MODULE
    ADC_If_Init();
#endif

#ifdef LED_MODULE
    LED_If_Init();
#endif

#ifdef KEY_MODULE
    Key_If_Init();
#endif

#ifdef DIAGNOSIS_MODULE
    Diagnosis_If_Init();
#endif

#ifdef EEPROM_MODULE
    EEPROM_If_Init();
#endif


}

/***********************************************************************************************
*
* @brief     From full operational level to not operational level, we will turn off backlight,turn off key,
                turn off LCD display
* @param    void
* @return   void
*
************************************************************************************************/
void Power_If_Full_Oper_To_Not_Operational(void)
{
#ifdef	LED_MODULE
    LED_If_Deinit();
#endif
#ifdef KEY_MODULE
    Key_If_Deinit();
#endif
#ifdef FTM_MODULE
    FTM_If_Deinit();
#endif  
}

/***********************************************************************************************
*
* @brief     from abnormal level to not operational level, turn on deserializer
* @param    void
* @return   void
*
************************************************************************************************/
void Power_If_Abnormal_To_Not_Operational(void)
{

#ifdef POWER_MANAGEMENT_MODULE
    Power_If_Init();
#endif

#ifdef CTP_MODULE
    CTP_If_Init();
#endif

#ifdef  KBI_MODULE
    KBI_If_Init();
#endif

#ifdef DESERIALIZE_MODULE
    Deserialize_If_Init();
#endif

#ifdef LCD_MODULE
    LCD_If_Init();
#endif

#ifdef ADC_MODULE
    ADC_If_Init();
#endif

#ifdef DIAGNOSIS_MODULE
    Diagnosis_If_Init();
#endif

#ifdef EEPROM_MODULE
    EEPROM_If_Init();
#endif
  
}

/***********************************************************************************************
*
* @brief     From full operational level to abnormal level, we will turn off backlight,turn off key,
                turn off LCD display, turn off CTP, turn off deserializer
* @param    void
* @return   void
*
************************************************************************************************/
void Power_If_Full_Oper_To_Abnormal(void)
{
#ifdef EEPROM_MODULE
        /* nothing to Deinit */
#endif

#ifdef DIAGNOSIS_MODULE
    Diagnosis_If_Init();/* the Deinit is same as Init*/
#endif

#ifdef KEY_MODULE
    Key_If_Deinit();
#endif

#ifdef	LED_MODULE
    LED_If_Deinit();
#endif

#ifdef  ADC_MODULE
    ADC_If_Deinit();
#endif  

#ifdef FTM_MODULE
    FTM_If_Deinit();
#endif

#ifdef LCD_MODULE
    LCD_If_Deinit();
#endif

#ifdef  DESERIALIZE_MODULE
    Deserialize_If_Deinit();
#endif  

#ifdef  KBI_MODULE
    KBI_If_Deinit();
#endif

#ifdef  CTP_MODULE
    CTP_If_Deinit();
#endif  

#ifdef POWER_MANAGEMENT_MODULE
    Power_If_Deinit();
#endif
}

/***********************************************************************************************
*
* @brief     From not operational level to abnormal level, we will turn off deserializer
* @param    void
* @return   void
*
************************************************************************************************/
void Power_If_Not_Operational_To_Abnormal(void)
{
#ifdef EEPROM_MODULE
        /* nothing to Deinit */
#endif

#ifdef DIAGNOSIS_MODULE
    Diagnosis_If_Init();/* the Deinit is same as Init*/
#endif
        
#ifdef  ADC_MODULE
    ADC_If_Deinit();
#endif  

#ifdef LCD_MODULE
    LCD_If_Deinit();
#endif

#ifdef  DESERIALIZE_MODULE
    Deserialize_If_Deinit();
#endif  

#ifdef  KBI_MODULE
    KBI_If_Deinit();
#endif

#ifdef  CTP_MODULE
    CTP_If_Deinit();
#endif  

#ifdef POWER_MANAGEMENT_MODULE
    Power_If_Deinit();
#endif
}
#endif /* POWER_MANAGEMENT_MODULE */
