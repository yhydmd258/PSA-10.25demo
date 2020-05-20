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
#include "../Timer/timer_if.h"
#include "power_if.h"
#include "power_ctrl.h"
#include "power_data.h"

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
typedef enum
{
    FULLY_OPERATION_LEVEL
    , OPERATION_KEEP_LEVEL
    , NOT_OPERATIONAL_LOW_LEVEL
    , NOT_OPERATIONAL_HIGH_LEVEL
    , ABNORMAL_LEVEL
} POWER_VOLTAGE_STATUS;

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static POWER_VOLTAGE_STATUS  Power_Voltage_Status = ABNORMAL_LEVEL;
static POWER_WAKEUP_STATUS  Power_Wakeup_Status = STANDBY_STATUS;
static uint8_t              Startup_Status;
static uint8_t              Startup_Success_Cmd[CMD_STARTUP_NUMBER_MAX]=
    {CMD_SEND_ID_SELF_CHECK_INFO,1,0,0,0,0,0,0,0};

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
void Power_Ctrl_Init(void)
{
    Startup_Status = FALSE;
    
    Power_Data_GPIO_Init();
}

/***********************************************************************************************
*
* @brief     Reinit the power manage module after exit from the standby mode
* @param    none
* @return   none
*
************************************************************************************************/
void Power_Ctrl_Deinit(void)
{
    Power_Data_GPIO_Deinit_Task();
}

/***********************************************************************************************
*
* @brief     read the status of wakeup
* @param    none
* @return   none
*
************************************************************************************************/
uint32_t Power_Ctrl_Wakeup_Status_Read(void)
{
    return Power_Wakeup_Status;
}

/***********************************************************************************************
*
* @brief     This function manages the power input voltage from the host 
* @param    none
* @return   none
*
************************************************************************************************/
void Power_Ctrl_Wakeup_Manage(void)
{
    uint32_t wakeup_pin_status;
    
    /* init wakeup pin as GPIO */
    Power_Data_Wakeup_GPIO_Init();

    /* read the wakeup pin status */
//    wakeup_pin_status = Power_Data_Wakeup_Pin_Read();
		wakeup_pin_status = 0;
    switch(Power_Wakeup_Status)
    {
        case ACTIVE_STATUS:
            Power_Ctrl_Voltage_Input_Manage(Power_If_Voltage_ADC());
            /* it will enter into standby status */
            if(wakeup_pin_status)
            {
                Power_Wakeup_Status = ACTIVE_TO_STANDBY;
            }
            break;
        case ACTIVE_TO_STANDBY:
            /* temp code, CTP enter into deep sleep */
            /* if the module go to standby status, the action is same as the action of voltage in  "ABNORMAL_LEVEL" */
            Power_Ctrl_Voltage_Input_Manage(POWER_INPUT_ADC_5V-1);
            Power_Wakeup_Status = STANDBY_ENTERING;
            break;
        case STANDBY_ENTERING:
            /* excute the deinit seuqence */
            Power_Data_GPIO_Deinit_Task();
            /* Deinit is OK */
            if(GPIO_DEINIT_END == Power_Data_Deinit_Status_Read())
            {
                /* record the wakeup status */
                Power_Wakeup_Status = STANDBY_STATUS;
                /* clear the startup status */
                Startup_Status = FALSE;
                /* init wakeup pin as KBI */
                Power_If_Wakeup_KBI_Init();
                /* enter into MCU stop mode */
                Power_Data_MCU_Stop();
            }
            break;
        case STANDBY_STATUS:
            /* if wakeup pin is high and 'Power_Wakeup_Status' is standby status, the module must go to active status */
            if(!wakeup_pin_status )
                Power_Wakeup_Status = STANDBY_TO_ACTIVE;
            break;
        case STANDBY_TO_ACTIVE:
            Power_Ctrl_Voltage_Input_Manage(Power_If_Voltage_ADC());
            Power_Wakeup_Status = ACTIVE_STATUS;
            break;
        default:
            break;
    }
}

/***********************************************************************************************
*
* @brief     This function manages the power input voltage from the host 
* @param    none
* @return   none
*
************************************************************************************************/
void Power_Ctrl_Voltage_Input_Manage(uint32_t power_ADC)
{
    POWER_VOLTAGE_STATUS power_status_temp = OPERATION_KEEP_LEVEL;
    
    /* Different power voltage is corresponding to different status */
    if((power_ADC > POWER_INPUT_ADC_8_5V) && (power_ADC < POWER_INPUT_ADC_18_2V))
    {
        power_status_temp = FULLY_OPERATION_LEVEL;
    }
    else if((power_ADC >= POWER_INPUT_ADC_5V) && (power_ADC < POWER_INPUT_ADC_7V))
    {
        power_status_temp = NOT_OPERATIONAL_LOW_LEVEL;
    }
    else if((power_ADC > POWER_INPUT_ADC_18_7V) && (power_ADC <= POWER_INPUT_ADC_26_5V))
    {
        power_status_temp = NOT_OPERATIONAL_HIGH_LEVEL;
    }
    else if((power_ADC < POWER_INPUT_ADC_5V) || (power_ADC > POWER_INPUT_ADC_26_5V))
    {
        power_status_temp = ABNORMAL_LEVEL;
    }
    else
    {
        /* operation keep level: 7V<= power_ADC <=8.5V, or 18.2V<=power_ADC<26.5V */
    }

    /* if the power status change, we will execute the action */
    if(power_status_temp != Power_Voltage_Status)
    {
        /* In different power voltage, execute different function */
        switch(power_status_temp)
        {
            case FULLY_OPERATION_LEVEL:
                if((NOT_OPERATIONAL_LOW_LEVEL == Power_Voltage_Status)
                    ||(NOT_OPERATIONAL_HIGH_LEVEL == Power_Voltage_Status))
                {
                    Power_If_Not_Oper_To_Full_Oper();
                }
                else
                {
                    Power_If_Abnormal_To_Full_Oper();
                }
                /* updata the status */
                Power_Voltage_Status = power_status_temp;
                break;
            case NOT_OPERATIONAL_LOW_LEVEL:
            case NOT_OPERATIONAL_HIGH_LEVEL:
                if(FULLY_OPERATION_LEVEL == Power_Voltage_Status )
                {
                    Power_If_Full_Oper_To_Not_Operational();
                }
                else
                {
                    Power_If_Abnormal_To_Not_Operational();
                }
                /* updata the status */
                Power_Voltage_Status = power_status_temp;
                break;
            case ABNORMAL_LEVEL:
                if((NOT_OPERATIONAL_LOW_LEVEL == Power_Voltage_Status)
                    ||(NOT_OPERATIONAL_HIGH_LEVEL == Power_Voltage_Status))
                {
                    Power_If_Not_Operational_To_Abnormal();
                }
                else
                {
                    Power_If_Full_Oper_To_Abnormal();
                }
                /* updata the status */
                Power_Voltage_Status = power_status_temp;
                break;
            case OPERATION_KEEP_LEVEL:
            default:
                break;
        }
    }
    else
    {
        /* do nothing */
    }
}

/***********************************************************************************************
*
* @brief     analyse the command
* @param    cmd_buf: the pointer of command
* @return   none
*
************************************************************************************************/
void Power_Ctrl_Cmd_Analyse(uint8_t *cmd_buf)
{
    switch(cmd_buf[0])
    {
        case CMD_REV_ID_STARTUP_DONE:
            if(FALSE== Startup_Status)
            {
                Startup_Status = TRUE;
//                Timer_If_Set(EN_SYSTEM_STARTUP_CMD_TIMER, TIMER_UP);
            }
            break;
        default:
            break;
    }
}

/***********************************************************************************************
*
* @brief     make the command
* @param    none
* @return   none
*
************************************************************************************************/
void Power_Ctrl_Startup_Cmd_Make(void)
{
    /* send the startup  command */
    Power_If_Startup_Cmd_Send(Startup_Success_Cmd, CMD_STARTUP_NUMBER_MAX);
    /* start timing, if more than 2s, this command is used for startup command */
    Timer_If_Set(EN_SYSTEM_STARTUP_CMD_TIMER, TIMER_COUNT_2S);
    /* this timer is used for INT PIN */
    Timer_If_Set(EN_DESER_SEND_DATA_TIMER, TIMER_COUNT_100MS);
}

/***********************************************************************************************
*
* @brief     read the state of the startup command communication
* @param    none
* @return   TRUE: the state of startup communication machine is OK
                FALSE:  the state of startup communication machine is going or flase.
*
************************************************************************************************/
uint8_t Power_Ctrl_Startup_Status_Read(void)
{
    if((FALSE==Startup_Status) && (TRUE==Timer_If_Check(EN_SYSTEM_STARTUP_CMD_TIMER))
        &&(ABNORMAL_LEVEL != Power_Voltage_Status))
//    if((FALSE==Startup_Status) && (TRUE==Timer_If_Check(EN_SYSTEM_STARTUP_CMD_TIMER))
//       &&(ACTIVE_STATUS == Power_Wakeup_Status))
    {
        Power_Ctrl_Startup_Cmd_Make();
    }
    return Startup_Status;
}
#endif /* POWER_MANAGEMENT_MODULE */
