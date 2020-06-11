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
#ifdef  CTP_MODULE
    
#include "CTP_if.h"
#include "CTP_ctrl.h"
#include "CTP_data.h"

#include "../Command/command.h"
#include "../Timer/timer_if.h"
#include <string.h>

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/
// static uint8_t Touch_Int = false;

/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void CTP_Ctrl_Ground_Calibrate(void);
static void CTP_Ctrl_MMI_Calibrate_Rps(uint8_t rps_value);

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
typedef enum
{
    OPERATING_MODE
   ,SYSTEM_INFOR_MODE   /*DAFAULT*/
   ,CFG_AND_TEST_MODE
}CYPRESS_IC_APP_MODE;

typedef enum
{
    CTP_GROUND_CALIBRATE    =0x00
    ,CTP_END_ENUM
}CTP_ANALYSE_ENUM;

typedef enum
{
    NO_ACTIVE
   ,BOOT_STATUS
   ,APP_SYSTEM_INFOR_MODE
   ,APP_OPERATING_MODE
   ,APP_CFG_AND_TEST_MODE
}CYPRESS_IC_ACTIVE_STATUS;

typedef enum
{
    MUTUAL_CAP_CALIBRATION_STATUS   =0x00   /* send mutual cap calibration command */
    ,BUTTOMS_CALIBRATION_STATUS              /* wait for the response of mutual cap command and send the buttoms calibration command */
    ,SELF_CAP_CALIBRATION_STATUS             /* wait for the response of buttoms calibration and send the self cap calibration command */
    ,CALIBRATION_WAITING_STATUS              /* wait for the response of self cap calibration */
    ,CALIBRATION_END
}CTP_GROUND_CALIBRATE_STATUS;

typedef enum
{
    EXIT_BOOT_WITH_CRC
   ,ENTER_SYSTEM_INFOR_MODE
   ,ENTER_OPERATING_MODE
   ,ENTER_CFG_AND_TEST_MODE
   ,COMMAND_END
}CMD_TYPE;

/**********************************************************************************************
* Local types
**********************************************************************************************/
static TOUCH_COORDINATE_BUF Touch_Report_Buf;
static uint32_t Touch_Version;  /* record the version of the CTP */
static uint8_t  Touch_Fault; /* TRUE: OK;  FALSE: abnormal */
static uint8_t CTP_Int_Flag=0;
static CYPRESS_IC_ACTIVE_STATUS CTP_Active_Status = NO_ACTIVE;
static CTP_GROUND_CALIBRATE_STATUS CTP_Calibrate_Status = CALIBRATION_END;

#define CTP_EXIT_BOOT_WITH_CRC_SIZE    0x09
#define CTP_ENTER_SYSTEM_INFOR_SIZE   0x02
#define CTP_ENTER_OPERATING_SIZE   0x02
#define CTP_ENTER_CFG_AND_TEST_SIZE   0x02

static uint8_t CTP_Command_Buf[COMMAND_END][CTP_EXIT_BOOT_WITH_CRC_SIZE]=
{
    /*Address,Host Sync,Packet Start,Command,Data Length LSB,Data Length MSB,CRC LSB,CRC MSB,Packet End */
   {0x00,0xFF,0x01,0x3B,0x00,0x00,0x4F,0x6D,0x17}  /*EXIT_BOOT_WITH_CRC*/
    /*Register Address, HST_MODE*/
   ,{0x00,0x10}  /*ENTER_SYSTEM_INFOR_MODE*/
   ,{0x00,0x00}  /*ENTER_OPERATING_MODE*/
   ,{0x00,0x20}  /*ENTER_CFG_AND_TEST_MODE*/
};

#define CTP_CALIBRATE_CMD_SIZE   0x05

/* in the configuration and test mode, send calibration command */
static uint8_t CTP_Calibrate_CMD_Buf[CALIBRATION_WAITING_STATUS][CTP_CALIBRATE_CMD_SIZE]=
{
    /* start address, hst_mode, RESET_DETECT, COMMAND, CMD_DATA */
    {0x00,0x20,0x00,0x09,0x00}/* MUTUAL_CAP_CALIBRATION_STATUS */
    ,{0x00,0x20,0x00,0x09,0x01}/* BUTTOMS_CALIBRATION_STATUS */
    ,{0x00,0x20,0x00,0x09,0x02}/* SELF_CAP_CALIBRATION_STATUS */
};

static uint8_t CTP_Analyse_Req[CTP_END_ENUM] = {FALSE};
static CALLBACK_FUNCTION CTP_Analyse_Function[CTP_END_ENUM] =
{
     &CTP_Ctrl_Ground_Calibrate  /* CTP_GROUND_CALIBRATE */
};

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/


/**********************************************************************************************
* Global functions
**********************************************************************************************/
/***********************************************************************************************
*
* @brief    initialize the CTP module
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Init(void)
{
#ifdef GEELY_SX11
    /* power on enable */
    CTP_Data_Power_En_Init();
    Timer_If_Delay(TIMER_DELAY_2MS);
#endif
    /* init I2C */
    CTP_Data_Init();
    /* power off CTP IC, set power level low */
    CTP_Data_Reset_Set(0);
    CTP_Active_Status = NO_ACTIVE;
    Timer_If_Set(EN_CTP_IC_STARTUP_TIMER, TIMER_COUNT_40MS);
//    Timer_If_Delay(TIMER_DELAY_40MS);
//    CTP_Data_Reset_Set(1);
    CTP_Ctrl_Int_Flag_Set(0);
//    Timer_If_Delay(TIMER_DELAY_100MS);

    Touch_Report_Buf.write_pos=0;
    Touch_Report_Buf.read_pos=COORDINATE_BUF_NUMBER-1;
    memset(Touch_Report_Buf.buf, 0x00, sizeof(TOUCH_REPORT_STRUCT));

    Touch_Version = 0;

    Touch_Fault = TRUE;
}

/***********************************************************************************************
*
* @brief    Deinitialize the CTP module
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Deinit(void)
{
#ifdef GEELY_SX11
    CTP_Data_Power_En_Deinit(); /* disenable the CTP power */
#else
    #define DEEP_SLEEP_SNED_SIZE    2
    uint8_t send_buf[DEEP_SLEEP_SNED_SIZE] = {0x00, 0x00};
    
    CTP_Data_Master_Read(0x00, &(send_buf[1]), 1);
    send_buf[1] |= DEEP_SLEEP_MASK;
    CTP_Data_Master_Send(send_buf, DEEP_SLEEP_SNED_SIZE);
#endif

//    CTP_Data_Master_Read(0x00, &(send_buf[1]), 1);

    /* clear the times which reading the message from CTP IC*/
    CTP_Ctrl_Int_Flag_Set(0);
    
    CTP_Data_Deinit();

    CTP_Active_Status = NO_ACTIVE;

    Timer_If_Clear(EN_CTP_IC_STARTUP_TIMER);
}

/***********************************************************************************************
*
* @brief    read the message from the touch IC
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Status_Check(void)
{
    if((NO_ACTIVE == CTP_Active_Status) && (TRUE == Timer_If_Check(EN_CTP_IC_STARTUP_TIMER)))
    {
        /* power on CTP IC, set power level High */
        CTP_Data_Reset_Set(1);
        CTP_Active_Status = BOOT_STATUS;
        Touch_Fault = TRUE;
    }
}

/***********************************************************************************************
*
* @brief    check the CTP is OK or not.
* @param    none
* @return   TRUE: OK
*               FALSE: abnormal
*
************************************************************************************************/
uint8_t CTP_Ctrl_Fault_Check(void)
{
    return Touch_Fault;
}

/***********************************************************************************************
*
* @brief    return the version of the touch IC
* @param    none
* @return   uint32_t&0xFF000000 : Firmware Major
                uint32_t&0x00FF0000 : Firmware Minor
                uint32_t&0x0000FF00 : Configuration version MSB
                uint32_t&0x000000FF : Configuration version LSB
*
************************************************************************************************/
uint32_t CTP_Ctrl_Version_Read(void)
{
    return Touch_Version;
}

/***********************************************************************************************
*
* @brief    analyse the command
* @param    cmd_buf: the pointer of command
                  cmd_size: command size
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Cmd_Analyse(uint8_t *cmd_buf, uint8_t cmd_size)
{
    #define GROUND_CALIBRATE_REQ    0x01
    #define CTP_RESTART             0x03
    
    switch(cmd_buf[1])
    {
        case CMD_TYPE_CTP_CALIBRATE:     /* QAC-9.6.0-0597 severity 5, ingore it */
            if(GROUND_CALIBRATE_REQ == cmd_buf[2])
            {
                CTP_Analyse_Req[CTP_GROUND_CALIBRATE] = TRUE;
            }
            else if(CTP_RESTART == cmd_buf[2])
            {
                /* set SYSRESETREQ bit, activate external SYSRESETREQ signal  */
                NVIC_SystemReset();
            }
            else
            {
                /* do nothing */
            }
            break;
        default:
            break;
    }
}

/***********************************************************************************************
*
* @brief    read the message from the touch IC
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Msg_Read(void)
{
#define FW_VER_MAJOR_POS    0x1C
#define FW_VER_MINOR_POS    0x1D
#define CYDATA_OFSH_POS     0x04
#define CYDATA_OFSL_POS     0x05
#define CYITO_OFFSET        0x15
#define MFGID_SZ_POS         0x22
#define READ_BUF_SIZE       133

    uint8_t   temp_buf[READ_BUF_SIZE]={0};
    uint8_t   app_mode=SYSTEM_INFOR_MODE;
    uint8_t   temp_data=0;
    uint8_t   touch_report_addr=0;
    uint8_t   rep_len=0;
    uint8_t   report_number=0;
    uint8_t   i=0;
    HST_MODE_STRUCT   *hst_mode=NULL;
//    uint8_t   reset_detect=0;
    TT_STAT_STRUCT   *tt_stat=NULL;
    TOUCH_REPORT_STRUCT *touch_report_infor=NULL;
    uint8_t   write_pos=0;
    uint16_t  cyito_pos=0;
#ifndef GEELY_SX11                
    uint8_t   buttons=0;
#endif

    if(CTP_Ctrl_Int_Flag_Get())
    {
        /* reduce the interrupt times */
        CTP_Ctrl_Int_Flag_Set(0);
//        CTP_Int_Flag--;
        /* read the touch information */
        if(CTP_Data_Master_Read(0x00, temp_buf, 133))
            return;
        /* analyse HST_MODE and RESET_DETECT */
        hst_mode = (HST_MODE_STRUCT*)(&temp_buf[HST_MODE]);
//        reset_detect = temp_buf[RESET_DETECT];

        if(NO_ACTIVE!=CTP_Active_Status)
        {
            // temp code, if CTP_Active_Status = BOOT_STATUS, the CTP IC reset, how to do.
            if(BOOT_STATUS!=CTP_Active_Status)
            {
                /* check the CTP IC is normal or abnormal*/
                if((!hst_mode->data_toggle &&(APP_OPERATING_MODE==CTP_Active_Status))
                    || (hst_mode->reset && temp_buf[RESET_DETECT]))
                {
                    /* power off CTP IC, set power level low */
                    CTP_Data_Reset_Set(0);
                    CTP_Active_Status = NO_ACTIVE;
                    Timer_If_Set(EN_CTP_IC_STARTUP_TIMER, TIMER_COUNT_40MS);
                    Touch_Fault = FALSE;
                }
                else
                {
                    /* check mode */
                    app_mode = hst_mode->device_mode;
                    switch(app_mode)
                    {
                        case OPERATING_MODE:    /* QAC-9.6.0-0597 severity 5, ingore it */ /* QAC-9.6.0-0570 severity 5, ingore it */
                            CTP_Active_Status = APP_OPERATING_MODE;
                            break;
                        case CFG_AND_TEST_MODE: /* QAC-9.6.0-0597 severity 5, ingore it */ /* QAC-9.6.0-0570 severity 5, ingore it */
                            CTP_Active_Status = APP_CFG_AND_TEST_MODE;
                            break;
                        case SYSTEM_INFOR_MODE: /* QAC-9.6.0-0597 severity 5, ingore it */ /* QAC-9.6.0-0570 severity 5, ingore it */
                        default:
                            CTP_Active_Status = APP_SYSTEM_INFOR_MODE;
                            break;
                    }
                }
            }
        }
        else
        {
            if(TRUE == Timer_If_Check(EN_CTP_IC_STARTUP_TIMER))
            {
                /* power on CTP IC, set power level High */
                CTP_Data_Reset_Set(1);
                CTP_Active_Status = BOOT_STATUS;
                Touch_Fault = TRUE;
            }
        }

        /* CTP IC work in normal state, not in bootloader state */
        switch(CTP_Active_Status)
        {
            case APP_OPERATING_MODE:
#ifndef GEELY_SX11                
                /* analyse the button */
                buttons = temp_buf[BUTTONS];
                /* Volume+ set */
                if(buttons & VOLUME_INCREASE_BUTTON)
                    CTP_If_Vol_Increase_Set(1);
                else
                    CTP_If_Vol_Increase_Set(0);
                /* Volume+ set */
                if(buttons & VOLUME_DECREASE_BUTTON)
                    CTP_If_Vol_Decrease_Set(1);
                else
                    CTP_If_Vol_Decrease_Set(0);
#endif                
                /* analyse the touch report */
                rep_len = temp_buf[REP_LEN];
                tt_stat = (TT_STAT_STRUCT*)&temp_buf[TT_STAT];
                if((0 != rep_len) && ( tt_stat->touch_record_number>0))
                {
                    /* get valid report number */
                    report_number = (tt_stat->touch_record_number>TOUCH_FINGER_NUMBER_MAX) ? TOUCH_FINGER_NUMBER_MAX : tt_stat->touch_record_number;
                    for(i=0; i<report_number; i++)
                    {
                        touch_report_addr = TOUCH_REPORT+i*TOUCH_REPORT_SIZE;
                        touch_report_infor = (TOUCH_REPORT_STRUCT*)&(temp_buf[touch_report_addr]);
                        if((touch_report_infor->touch_x_MSB>0)
                            ||(touch_report_infor->touch_x_LSB>0)
                            ||(touch_report_infor->touch_y_MSB>0)
                            ||(touch_report_infor->touch_y_LSB>0))
                        {
                            /* if the state is no event, ignore the information, */
                            if(touch_report_infor->touch_ID_str.event_ID!= NO_EVENT)
                            {
                                /* if the buf is full, ignore the touch report */
                                write_pos = (Touch_Report_Buf.write_pos+1)%COORDINATE_BUF_NUMBER;
                                if(write_pos != Touch_Report_Buf.read_pos)
                                {
                                    memcpy((uint8_t*)&(Touch_Report_Buf.buf[Touch_Report_Buf.write_pos]),&temp_buf[touch_report_addr],TOUCH_REPORT_SIZE);
                                    Touch_Report_Buf.write_pos = write_pos;
                                }
                                else
                                {
                                    break;
                                }
                            }
                            else
                            {
                                /*do nothing, enter next cycle */
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                break;
            case BOOT_STATUS:
                CTP_Data_Master_Send(CTP_Command_Buf[EXIT_BOOT_WITH_CRC],CTP_EXIT_BOOT_WITH_CRC_SIZE);
                CTP_Active_Status = APP_SYSTEM_INFOR_MODE;
                break;
            case APP_SYSTEM_INFOR_MODE:
                Touch_Version = 0;
                /* read the firmware version */
                Touch_Version += temp_buf[FW_VER_MAJOR_POS]<<24;
                Touch_Version += temp_buf[FW_VER_MINOR_POS]<<16;
                /* read the configurature version */
                cyito_pos = temp_buf[CYDATA_OFSH_POS]<<8;
                cyito_pos += temp_buf[CYDATA_OFSL_POS];
                cyito_pos += temp_buf[MFGID_SZ_POS]+CYITO_OFFSET;
                cyito_pos &= 0xFF;
                cyito_pos = (cyito_pos<READ_BUF_SIZE)? cyito_pos : READ_BUF_SIZE;
                Touch_Version += temp_buf[cyito_pos]<<8;
                Touch_Version += temp_buf[cyito_pos+1];
                /* request the CTP IC to switch mode */
                CTP_Command_Buf[ENTER_OPERATING_MODE][1]|=(temp_data|DATA_TOGGLE_MASK|MODE_CHANGE_RQ_MASK);
                CTP_Data_Master_Send(CTP_Command_Buf[ENTER_OPERATING_MODE],CTP_ENTER_OPERATING_SIZE);
                CTP_Active_Status = APP_OPERATING_MODE;
                break;
            case APP_CFG_AND_TEST_MODE:
                if(CALIBRATION_END == CTP_Calibrate_Status)
                {
                    /* clear data(bit4-6: device mode) */
                    //                temp_data = hst_mode[0]&(~DEVICE_MODE_MASK);
                    /* set bit7(1), set bit3(1), set bit2(1) */
                    //                CTP_Command_Buf[ENTER_OPERATING_MODE][1]|=(temp_data|DATA_TOGGLE_MASK|MODE_CHANGE_RQ_MASK|LOW_POWER_ENABLE_MASK);
                    CTP_Command_Buf[ENTER_OPERATING_MODE][1]|=(temp_data|DATA_TOGGLE_MASK|MODE_CHANGE_RQ_MASK);
                    CTP_Data_Master_Send(CTP_Command_Buf[ENTER_OPERATING_MODE],CTP_ENTER_OPERATING_SIZE);
                    CTP_Active_Status = APP_OPERATING_MODE;
                }
                else
                {
                    /* the ground calibration function is only used during the factory prodution */
                    switch(CTP_Calibrate_Status)
                    {
                        case MUTUAL_CAP_CALIBRATION_STATUS:
                            /* send the mutual cap calibration command to CTP IC, then wait for a moment, and read the response */
                            if(!CTP_Data_Master_Send(CTP_Calibrate_CMD_Buf[MUTUAL_CAP_CALIBRATION_STATUS],CTP_CALIBRATE_CMD_SIZE))
                            {
                                CTP_Calibrate_Status = BUTTOMS_CALIBRATION_STATUS;
                            }
                            else
                            {
                                /* send the error response */
                                CTP_Ctrl_MMI_Calibrate_Rps(1);
                                CTP_Calibrate_Status = CALIBRATION_END;
                            }
                            break;
                        case BUTTOMS_CALIBRATION_STATUS:
                            /* read the response of mutual cap  calibration */
                            if(!CTP_Data_Master_Read(0x00, temp_buf, 4))
                            {
                                /* if the value of temp_buf[3] is 0, it indicates calibration is ok; Other value indicates error */
                                if((0 == temp_buf[3]) &&
                                    (!CTP_Data_Master_Send(CTP_Calibrate_CMD_Buf[BUTTOMS_CALIBRATION_STATUS],CTP_CALIBRATE_CMD_SIZE)))
                                {
                                    CTP_Calibrate_Status = SELF_CAP_CALIBRATION_STATUS;
                                }
                                else
                                {
                                    /* send the error response */
                                    CTP_Ctrl_MMI_Calibrate_Rps(1);
                                    CTP_Calibrate_Status = CALIBRATION_END;
                                }
                            }
                            else
                            {
                                /* send the error response */
                                CTP_Ctrl_MMI_Calibrate_Rps(1);
                                CTP_Calibrate_Status = CALIBRATION_END;
                            }
                            break;
                        case SELF_CAP_CALIBRATION_STATUS:
                            /* read the response of mutual cap  calibration */
                            if(!CTP_Data_Master_Read(0x00, temp_buf, 4))
                            {
                                /* if the value of temp_buf[3] is 0, it indicates calibration is ok; Other value indicates error */
                                if((0 == temp_buf[3]) &&
                                    (!CTP_Data_Master_Send(CTP_Calibrate_CMD_Buf[SELF_CAP_CALIBRATION_STATUS],CTP_CALIBRATE_CMD_SIZE)))
                                {
                                    CTP_Calibrate_Status = CALIBRATION_WAITING_STATUS;
                                }
                                else
                                {
                                    /* send the error response */
                                    CTP_Ctrl_MMI_Calibrate_Rps(1);
                                    CTP_Calibrate_Status = CALIBRATION_END;
                                }
                            }
                            else
                            {
                                /* send the error response */
                                CTP_Ctrl_MMI_Calibrate_Rps(1);
                                CTP_Calibrate_Status = CALIBRATION_END;
                            }
                            break;
                        case CALIBRATION_WAITING_STATUS:
                            /* read the response of mutual cap  calibration */
                            if(!CTP_Data_Master_Read(0x00, temp_buf, 4))
                            {
                                /* if the value of temp_buf[3] is 0, it indicates calibration is ok; Other value indicates error */
                                if(0 == temp_buf[3])
                                {
                                    /* send the positive response */
                                    CTP_Ctrl_MMI_Calibrate_Rps(0);
                                }
                                else
                                {
                                    /* send the error response */
                                    CTP_Ctrl_MMI_Calibrate_Rps(1);
                                }
                            }
                            else
                            {
                                /* send the error response */
                                CTP_Ctrl_MMI_Calibrate_Rps(1);
                            }
                            CTP_Calibrate_Status = CALIBRATION_END;
                            break;
                        case CALIBRATION_END:
                        default:
                            break;
                    }
                }
                break;
            case NO_ACTIVE:
            default:
                break;
        }
    }
    else
    {
        // there is no interrupt, wait for next cycle.
    }
}

/***********************************************************************************************
*
* @brief     make the command and send to deserializer module
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Cmd_Make(void)
{
    CTP_Data_Cmd_Make(&Touch_Report_Buf);
}

/***********************************************************************************************
*
* @brief      save the command data which will be sent
* @param     data: the data need to be made into standard frame
                data_size:  the size of the data need to be made into standard frame
                repeat: the repeat times of the data need to be sent
                extend_data: the data need to be made into extended frame
                extend_size: the size of the data need to be made into extended frame
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Cmd_Send_Save(uint8_t* data,uint8_t data_size,uint8_t repeat,uint8_t *extend_data,uint8_t extend_size)
{
    CTP_If_Cmd_Send_Save(data, data_size, repeat, extend_data, extend_size);
}

/***********************************************************************************************
*
* @brief    set the CTP interrupt flag
* @param    flag : 1 the flag of reading data from the CTP IC
                        0 (after reading the message from the CTP IC, clear the flag )
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Int_Flag_Set(uint8_t flag)
{
    CTP_Int_Flag = flag;
}

/***********************************************************************************************
*
* @brief    get the flag of the CTP interrupt
* @param    none
* @return   the flag of the CTP interrupt
*
************************************************************************************************/
uint8_t CTP_Ctrl_Int_Flag_Get(void)
{
    return CTP_Int_Flag;
}

/***********************************************************************************************
*
* @brief    response the command received from the MMI
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Cmd_Response(void)
{
    uint8_t fun_number=0;

    for(fun_number=0; fun_number<CTP_END_ENUM; fun_number++)
    {
        if((TRUE == CTP_Analyse_Req[fun_number]) && CTP_Analyse_Function[fun_number])
        {
            CTP_Analyse_Function[fun_number]();
        }
    }
}

/***********************************************************************************************
*
* @brief    start the status of the ground calibrate about CTP IC
* @param    none
* @return   none
*
************************************************************************************************/
static void CTP_Ctrl_Ground_Calibrate(void)
{
    if(APP_OPERATING_MODE == CTP_Active_Status)
    {
        /* wait the CTP IC to switch to the operating mode, and then transfer to the configuation and test mode */
        CTP_Command_Buf[ENTER_CFG_AND_TEST_MODE][1]|=DATA_TOGGLE_MASK|MODE_CHANGE_RQ_MASK;
        CTP_Data_Master_Send(CTP_Command_Buf[ENTER_CFG_AND_TEST_MODE],CTP_ENTER_CFG_AND_TEST_SIZE);
        /* request to calibrate the ground of CTP IC*/
        CTP_Calibrate_Status = MUTUAL_CAP_CALIBRATION_STATUS; 
        CTP_Analyse_Req[CTP_GROUND_CALIBRATE] = FALSE;
    }
}

/***********************************************************************************************
*
* @brief    response the command from the MMI
* @param    none
* @return   0: request to send positive response to MMI
                1: request to send nagative response to MMI
*
************************************************************************************************/
static void CTP_Ctrl_MMI_Calibrate_Rps(uint8_t rps_value)
{
    #define CALIBRATE_CMD_BUF_SIZE      9
    #define CALIBRATE_RESULT_SUCCESS    1
    #define CALIBRATE_RESULT_ERROR      0
    
    uint8_t response_buf[CALIBRATE_CMD_BUF_SIZE] = {0};

    response_buf[0] = CMD_SEND_ID_USED_FACTORY;
    response_buf[1] = CMD_TYPE_CTP_CALIBRATE;

    if(rps_value)
    {
        response_buf[2] = CALIBRATE_RESULT_ERROR;
    }
    else
    {
        response_buf[2] = CALIBRATE_RESULT_SUCCESS;
    }

    /* send the command to MMI */
    CTP_Ctrl_Cmd_Send_Save(response_buf, CALIBRATE_CMD_BUF_SIZE, 0, NULL, 0);
}

#endif /* CTP_MODULE */
