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
#ifdef  DIAGNOSIS_MODULE
#include <string.h>
#include "../Command/command.h"
#include "diagnosis_if.h"
#include "diagnosis_ctrl.h"

#ifdef DESERIALIZE_MODULE
#include "../Deserialize/deserialize_if.h"
#endif
#ifdef	LED_MODULE
#include "../LED/LED_if.h"
#endif
#ifdef  LCD_MODULE
#include "../LCD/LCD_if.h"
#endif
#ifdef  CTP_MODULE
#include "../CTP/CTP_if.h"
#endif  
#ifdef ADC_MODULE
#include "../ADC/ADC_if.h"
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
* @brief    init the deserilize module
* @param    none
* @return   none
*
************************************************************************************************/
void Diagnosis_If_Init(void)
{
    Diagnosis_Ctrl_Init();
}

/***********************************************************************************************
*
* @brief    report the status of the module after the handshake protocol succeeded with MMI
* @param    none
* @return   none
*
************************************************************************************************/
void Diagnosis_If_First_Report(void)
{
    Diagnosis_Ctrl_Fun_Defect();
    Diagnosis_Ctrl_Voltage_Range();
    Diagnosis_Ctrl_Version();
}

/***********************************************************************************************
*
* @brief    task function
* @param    none
* @return   none
*
************************************************************************************************/
void Diagnosis_If_Task(void)
{
    Diagnosis_Ctrl_Cmd_Response();
}

/***********************************************************************************************
*
* @brief    analyse the command
* @param    cmd_buf: the pointer of command
* @return   none
*
************************************************************************************************/
void Diagnosis_If_Cmd_Analyse(uint8_t *cmd_buf)
{
    Diagnosis_Ctrl_Cmd_Analyse(cmd_buf);
}
/***********************************************************************************************
*
* @brief    the ADC value of the input voltage
* @param    none
* @return   unit8_t : the value of ADC
*
************************************************************************************************/
uint16_t Diagnosis_If_Volt_ADC_Get(void)
{
#ifdef	ADC_MODULE
    return ADC_If_Value_Get(ADC_IF_INPUT_VOL);
#else
    return VOLTAGE_ADC_12V;
#endif
}

/***********************************************************************************************
*
* @brief    save the command data which will be sent
* @param    data: the data need to be made into standard frame
                data_size:  the size of the data need to be made into standard frame
* @return   none
*
************************************************************************************************/
void Diagnosis_If_Cmd_Send_Save(uint8_t* data, uint8_t data_size)
{
#ifdef  DESERIALIZE_MODULE
        Deserialize_If_Send_Cmd_Save(data, data_size, 0, NULL, 0);    
#endif
}

/***********************************************************************************************
*
* @brief    check the LED status is OK or not.
* @param    none
* @return   TRUE: OK
*               FALSE: abnormal
*
************************************************************************************************/
uint8_t Diagnosis_If_LED_Check(void)
{
#ifdef  LED_MODULE
    return LED_If_Fault_Read();
#else
    return TRUE;
#endif
}

/***********************************************************************************************
*
* @brief    check the LCD status is OK or not.
* @param    none
* @return   TRUE: OK
*               FALSE: abnormal
*
************************************************************************************************/
uint8_t Diagnosis_If_LCD_Check(void)
{
#ifdef  LCD_MODULE
    return LCD_If_Fault_Check();
#else
    return TRUE;
#endif
}

/***********************************************************************************************
*
* @brief    check the CTP status is OK or not.
* @param    none
* @return   TRUE: OK
*               FALSE: abnormal
*
************************************************************************************************/
uint8_t Diagnosis_If_CTP_Check(void)
{
#ifdef  CTP_MODULE
    return CTP_If_Fault_Check();
#else
    return TRUE;
#endif
}

/***********************************************************************************************
*
* @brief    check the display temperature is OK or not.
* @param    none
* @return   TRUE: OK
*               FALSE: abnormal
*
************************************************************************************************/
uint8_t Diagnosis_If_Display_Temper_Check(void)
{
    /* temp code */
#ifdef	ADC_MODULE
        return TRUE;
#else
        return TRUE;
#endif
}
#endif/*DIAGNOSIS_MODULE*/
