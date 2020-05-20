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
#include "../Command/command.h"
#include "../Version/version.h"
#include "diagnosis_if.h"
#include "diagnosis_ctrl.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
typedef enum{
    DIAG_FUNCTION_DEFECT_REQ    =   0
    ,DIAG_VOLTAGE_RANGE_REQ
    ,DIAG_VERSION_REQ
    ,DIAG_REQ_END
}DIAG_CMD_RESPONSE_REQ;

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static uint8_t Status_Question_Req[DIAG_REQ_END] = {0};
static CALLBACK_FUNCTION Diag_Response_Function[DIAG_REQ_END] =
{
     &Diagnosis_Ctrl_Fun_Defect,    /* DIAG_FUNCTION_DEFECT_REQ */
     &Diagnosis_Ctrl_Voltage_Range, /* DIAG_VOLTAGE_RANGE_REQ */
     &Diagnosis_Ctrl_Version       /* DIAG_VERSION_REQ */
};

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
void Diagnosis_Ctrl_Init(void)
{
    uint8_t fun_number=0;

    for(fun_number=0; fun_number<DIAG_REQ_END; fun_number++)
    {
        Status_Question_Req[fun_number] =FALSE;
    }
}

/***********************************************************************************************
*
* @brief    response the command received from the MMI
* @param    none
* @return   none
*
************************************************************************************************/
void Diagnosis_Ctrl_Cmd_Response(void)
{
    uint8_t fun_number=0;

    for(fun_number=0; fun_number<DIAG_REQ_END; fun_number++)
    {
        if(TRUE == Status_Question_Req[fun_number])
        {
            Diag_Response_Function[fun_number]();
            
            /* clear the flag */
            Status_Question_Req[fun_number] = FALSE;
        }
    }
}
/***********************************************************************************************
*
* @brief    analyse the command
* @param    cmd_buf: the pointer of command
* @return   none
*
************************************************************************************************/
void Diagnosis_Ctrl_Cmd_Analyse(uint8_t *cmd_buf)
{
    switch(cmd_buf[0])
    {
        case CMD_REV_ID_STATUS_QUESTION:
            Status_Question_Req[DIAG_FUNCTION_DEFECT_REQ] = TRUE;
            Status_Question_Req[DIAG_VOLTAGE_RANGE_REQ] = TRUE;
            Status_Question_Req[DIAG_VERSION_REQ] = TRUE;
            break;
        default:
            break;
    }
}

/***********************************************************************************************
*
* @brief    function defect, include the module status, display status, touch function, temperature status
* @param    none
* @return   none
*
************************************************************************************************/
void Diagnosis_Ctrl_Fun_Defect(void)
{
    #define FUNC_DEFECT_CMD_SIZE   0x09
    
    uint8_t send_cmd[FUNC_DEFECT_CMD_SIZE]={0};
    uint8_t LED_fault = 0;/* record the LED fault status */
    
    send_cmd[0] = CMD_SEND_ID_FUNCTION_DEFECT;
    /* display status(LCD), 0:ok, 1: abnormal */
    if(TRUE == Diagnosis_If_LCD_Check())
        send_cmd[2] = 0;
    else
        send_cmd[2] = 1;
    /* touch function, 0:ok, 1: abnormal */
    if(TRUE == Diagnosis_If_CTP_Check())
        send_cmd[3] = 0;
    else
        send_cmd[3] = 1;
    /* temperature status, 0:ok, 1: abnormal */
    if(TRUE == Diagnosis_If_Display_Temper_Check())
        send_cmd[4] = 0;
    else
        send_cmd[4] = 1;

    /* LED status, 0:ok, 1: abnormal */
    if(TRUE == Diagnosis_If_LED_Check())
        LED_fault = 0;
    else
        LED_fault = 1;
    
    /* the module status, 0:ok, 1: abnormal */
    send_cmd[1] = LED_fault | send_cmd[2] | send_cmd[3] | send_cmd[4];
    /* send data */
    Diagnosis_If_Cmd_Send_Save(send_cmd, FUNC_DEFECT_CMD_SIZE);
}

/***********************************************************************************************
*
* @brief    defect voltage range
* @param    none
* @return   none
*
************************************************************************************************/
void Diagnosis_Ctrl_Voltage_Range(void)
{
    #define VOLTAGE_RANGE_CMD_SIZE   0x09
    
    uint8_t send_cmd[VOLTAGE_RANGE_CMD_SIZE]={0};
    uint16_t voltage_adc = 0;
    
    send_cmd[0] = CMD_SEND_ID_VOLTAGE_RANGE;
    /* input voltage status, 0:(10V-15V), 1:(15V-16V), 2:(9V-10V), 3:other */
    voltage_adc = Diagnosis_If_Volt_ADC_Get();
    if((VOLTAGE_ADC_10V<=voltage_adc) && (VOLTAGE_ADC_15V>=voltage_adc))
    {
        send_cmd[1] = 0;
    }
    else if((VOLTAGE_ADC_15V<voltage_adc) && (VOLTAGE_ADC_16V>voltage_adc))
    {
        send_cmd[1] = 1;
    }
    else if((VOLTAGE_ADC_9V<=voltage_adc) && (VOLTAGE_ADC_10V>voltage_adc))
    {
        send_cmd[1] = 2;
    }
    else
    {
        send_cmd[1] = 3;
    }
    
    /* send data */
    Diagnosis_If_Cmd_Send_Save(send_cmd, VOLTAGE_RANGE_CMD_SIZE);
}

/***********************************************************************************************
*
* @brief    defect Version
* @param    none
* @return   none
*
************************************************************************************************/
void Diagnosis_Ctrl_Version(void)
{
    #define VERSION_CMD_SIZE   0x09
    
    uint8_t send_cmd[VERSION_CMD_SIZE]={0};

    send_cmd[0] = CMD_SEND_ID_VERSION;
    /* HW Version number before the dot, for example, 1.2, send_cmd[1]=1, send_cmd[2]=2 */
    send_cmd[1] = HW_VER_NUMBER_BEFORE_DOT;
    send_cmd[2] = HW_VER_NUMBER_AFTER_DOT;
    /* SW Version number before the dot, for example, 1.2, send_cmd[3]=1, send_cmd[4]=2 */
    send_cmd[3] = SW_VER_NUMBER_BEFORE_DOT;
    send_cmd[4] = SW_VER_NUMBER_AFTER_DOT;
    /* Is SW UserProgram or BootProgram, 0:userprogram, 1:bootprogram */
    send_cmd[5] = 0;

    /* send data */
    Diagnosis_If_Cmd_Send_Save(send_cmd, VERSION_CMD_SIZE);
}

#endif/*DIAGNOSIS_MODULE*/
