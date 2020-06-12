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
    
#include <stdio.h>
#include <string.h>
#include "CTP_ctrl.h"
#include "CTP_data.h"
#include "Command/command.h"
#include "I2C.h"
#include "GPIO.h"

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
    UNGRIP_EVENT    =0x01
    , SUPPRESS_EVENT=0x02
    , AMP_EVENT     =0x04
    , VECTOR_EVENT =0x08
    , MOVE_EVENT    =0x10
    , RELEASE_EVENT =0x20
    , PRESS_EVENT   =0x40
    , DETECT_EVENT  =0x80
}TOUCH_EVENT_ID;

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
// static void CTP_Data_GPIO_Cfg(void);

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
void CTP_Data_Init(void)
{
    I2C_ConfigType  I2C_Config = {{0}};    
    /* Initialize I2C module with interrupt mode */
    I2C_Config.u16Slt = 0x0000;
//    I2C_Config.u16F = 0x0090; /* Baud rate at 100Kbps, MULT=4,  SCL divider=48, 20M/(MULT*divider) */
    I2C_Config.u16F = 0x0010; /* Baud rate at 400Kbps, MULT=1,  SCL divider=48, 20M/(MULT*divider) */
    I2C_Config.sSetting.bMSTEn = 1;
    I2C_Config.sSetting.bIntEn = 0;
    I2C_Config.sSetting.bI2CEn = 1;

    I2C_Init(I2C1, &I2C_Config);
//  I2C_ClearStatus(CTP_IC,I2C_S_IICIF_MASK);
}

/***********************************************************************************************
*
* @brief    Deinitialize the CTP module
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Deinit(void)
{
    I2C_Deinit(CTP_IC);
//    CTP_Data_Reset_Set(0);
//    OUTPUT_CLEAR(PTG,PTG1); 

//    CONFIG_PIN_AS_GPIO(PTE, PTE0, INPUT);
//    CONFIG_PIN_AS_GPIO(PTE, PTE1, INPUT);

//    CONFIG_PIN_AS_GPIO(PTG, PTG0, INPUT);
}

/***********************************************************************************************
*
* @brief    send the data to the CTP IC
* @param    send_data : the pointer of the send data
                 data_size : ths size of the send data
* @return   0: OK; other: error
*
************************************************************************************************/
uint8_t CTP_Data_Master_Send(uint8_t *send_data, uint32_t data_size)
{
    if((NULL==send_data) || (0 == data_size))
        return 1;

    /* send data */
//    I2C_IntDisable(CTP_IC);
    return I2C_MasterSendWait(CTP_IC,CTP_IC_SLAVE_ADDRESS,&send_data[0],data_size);
//    I2C_IntEnable(CTP_IC);
}
/***********************************************************************************************
*
* @brief    reg_addr : the address of the register
                read_data : the pointer of the read data
                data_size : the size of the read data
* @param    none
* @return   0: read OK
                other: read error
*
************************************************************************************************/
uint8_t CTP_Data_Master_Read(uint8_t reg_addr, uint8_t *read_data, uint32_t data_size)
{
    uint8_t result=1;
    
    if((NULL==read_data) || (0 == data_size))
        return result;
    
    result = I2C_MasterSendWait(CTP_IC,CTP_IC_SLAVE_ADDRESS,&reg_addr,1);
    if(I2C_ERROR_NULL == result)
        result = I2C_MasterReadWait(CTP_IC,CTP_IC_SLAVE_ADDRESS,read_data,data_size);
    
    return result;
}

/***********************************************************************************************
*
* @brief    the touch information is made the command according to the protocol of SX12  
* @param    touch_data : the pointer of touch information
* @return   none
*
************************************************************************************************/
void CTP_Data_Cmd_Make(TOUCH_COORDINATE_BUF *touch_data)
{
    #define COMMAND_SIZE_MAX            21
    #define EXTEND_COMMAND_SIZE_MAX    28
    #define EVENT_RELEASE_REPEAT_TIME   2
    
    uint8_t   cmd_data[COMMAND_SIZE_MAX]={0};
    uint8_t   extend_cmd_data[EXTEND_COMMAND_SIZE_MAX]={0};
    uint8_t   repeat_time=0;
    uint8_t   postion_offset=0;
    uint8_t   temp_read_position=0;
    uint8_t   number=0;
        
    if((touch_data->read_pos+1)%COORDINATE_BUF_NUMBER == touch_data->write_pos)
        return;

    /* start to communication with deserializer */
    while((touch_data->read_pos+1)%COORDINATE_BUF_NUMBER != touch_data->write_pos) 
    {
        repeat_time=0;

        memset(cmd_data, 0x00, COMMAND_SIZE_MAX);
        /* make  touch position info standard command */
        cmd_data[0] = CMD_SEND_ID_TP_POSITION;
        for(number=0; number<TP_INFO_NUMBER_MAX; number++)
        {
            /* the read position */
            temp_read_position = (touch_data->read_pos+1)%COORDINATE_BUF_NUMBER;
            if(temp_read_position != touch_data->write_pos)
            {
                if((LIFT_OFF==touch_data->buf[temp_read_position].touch_ID_str.event_ID)&&(0!=number))
                    break;
                /* record the number of touch report */
                cmd_data[1]++;
                /* calculate the offset */
                postion_offset = EVERY_TP_INFO_SIZE*number;
                /* byte2 + offset : touch ID for No.(1+number) touch
                                byte3 + offset : status for No.(1+number)
                                byte4 + offset : X position MSByte for No.(1+number)
                                byte5 + offset : X position LSByte for No.(1+number)
                                byte6 + offset : Y position MSByte for No.(1+number)
                                byte7 + offset : Y position LSByte for No.(1+number)
                                byte8 + offset : reserved, Size of touch for No.(1+number)
                                byte9 + offset : reserved, Touch amplitude for No.(1+number)
                                byte10 + offset : reserved, Touch vector for No.(1+number)
                                */
                cmd_data[2+postion_offset] = touch_data->buf[temp_read_position].touch_ID_str.touch_ID;
                switch(touch_data->buf[temp_read_position].touch_ID_str.event_ID)
                {
                    case TOUCH_DOWN:    /* QAC-9.6.0-0597 severity 5, ingore it */ /* QAC-9.6.0-0570 severity 5, ingore it */
                        cmd_data[3+postion_offset] = PRESS_EVENT;
                        break;
                    case LIFT_OFF:      /* QAC-9.6.0-0597 severity 5, ingore it */ /* QAC-9.6.0-0570 severity 5, ingore it */
                        cmd_data[3+postion_offset] = RELEASE_EVENT;
                        repeat_time = EVENT_RELEASE_REPEAT_TIME;
                        break;
                    case MOVING:        /* QAC-9.6.0-0597 severity 5, ingore it */ /* QAC-9.6.0-0570 severity 5, ingore it */
                    default:
                        cmd_data[3+postion_offset] = MOVE_EVENT;
                        break;
                }
                cmd_data[4+postion_offset] = touch_data->buf[temp_read_position].touch_x_MSB;
                cmd_data[5+postion_offset] = touch_data->buf[temp_read_position].touch_x_LSB;
                cmd_data[6+postion_offset] = touch_data->buf[temp_read_position].touch_y_MSB;
                cmd_data[7+postion_offset] = touch_data->buf[temp_read_position].touch_y_LSB;
                cmd_data[8+postion_offset] = 0;
                cmd_data[9+postion_offset] = 0;
                cmd_data[10+postion_offset] = 0;
                /* record the read position */
                touch_data->read_pos = temp_read_position;
            }
            else
            {
                break;
            }
        }
        /* reserved */
        cmd_data[COMMAND_SIZE_MAX-1] = 0;
        
        memset(extend_cmd_data, 0x00, EXTEND_COMMAND_SIZE_MAX);
        /* make tp position info extended command */
        extend_cmd_data[0] = CMD_SEND_ID_EXTENDED_TP_POS;
        for(number=0; number<EXTEND_TP_INFO_NUMBER_MAX; number++)
        {
            /* the read position */
            temp_read_position = (touch_data->read_pos+1)%COORDINATE_BUF_NUMBER;
            /* the key touch report must not be included in the extended touch information */
            if((temp_read_position != touch_data->write_pos)
                && (LIFT_OFF!=touch_data->buf[temp_read_position].touch_ID_str.event_ID) )
            {
                /* record the number of touch report */
                cmd_data[1]++;
                /* calculate the offset */
                postion_offset = EVERY_TP_INFO_SIZE*number;
                /* byte1 + offset : touch ID for No.(1+number) touch
                                byte2 + offset : status for No.(1+number)
                                byte3 + offset : X position MSByte for No.(1+number)
                                byte4 + offset : X position LSByte for No.(1+number)
                                byte5 + offset : Y position MSByte for No.(1+number)
                                byte6 + offset : Y position LSByte for No.(1+number)
                                byte7 + offset : reserved, Size of touch for No.(1+number)
                                byte8 + offset : reserved, Touch amplitude for No.(1+number)
                                byte9 + offset : reserved, Touch vector for No.(1+number)
                                */
                extend_cmd_data[1+postion_offset] = touch_data->buf[temp_read_position].touch_ID_str.touch_ID;
                switch(touch_data->buf[temp_read_position].touch_ID_str.event_ID)
                {
                    case TOUCH_DOWN:    /* QAC-9.6.0-0597 severity 5, ingore it */ /* QAC-9.6.0-0570 severity 5, ingore it */
                        extend_cmd_data[2+postion_offset] = PRESS_EVENT;
                        break;
                    case LIFT_OFF:      /* QAC-9.6.0-0597 severity 5, ingore it */ /* QAC-9.6.0-0570 severity 5, ingore it */
                        extend_cmd_data[2+postion_offset] = RELEASE_EVENT;
                        break;
                    case MOVING:        /* QAC-9.6.0-0597 severity 5, ingore it */ /* QAC-9.6.0-0570 severity 5, ingore it */
                    default:
                        extend_cmd_data[2+postion_offset] = MOVE_EVENT;
                        break;
                }
                extend_cmd_data[3+postion_offset] = touch_data->buf[temp_read_position].touch_x_MSB;
                extend_cmd_data[4+postion_offset] = touch_data->buf[temp_read_position].touch_x_LSB;
                extend_cmd_data[5+postion_offset] = touch_data->buf[temp_read_position].touch_y_MSB;
                extend_cmd_data[6+postion_offset] = touch_data->buf[temp_read_position].touch_y_LSB;
                extend_cmd_data[7+postion_offset] = 0;
                extend_cmd_data[8+postion_offset] = 0;
                extend_cmd_data[9+postion_offset] = 0;
                /* record the read position */
                touch_data->read_pos = temp_read_position;
            }
            else
            {
                break;
            }
        }
        /* send command */
        if(number>0)
            CTP_Ctrl_Cmd_Send_Save(cmd_data, COMMAND_SIZE_MAX, repeat_time, extend_cmd_data, EXTEND_COMMAND_SIZE_MAX);
        else
            CTP_Ctrl_Cmd_Send_Save(cmd_data, COMMAND_SIZE_MAX, repeat_time, NULL, 0);
    }
}

/***********************************************************************************************
*
* @brief    level : 1( CTP IC power on )
                        0( CTP IC power off )
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Reset_Set(uint8_t level)
{
    if(level)
        OUTPUT_SET(PTG,PTG1);
    else
        OUTPUT_CLEAR(PTG,PTG1);
}

/***********************************************************************************************
*
* @brief    CTP power enable
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Power_En_Init(void)
{
    CONFIG_PIN_AS_GPIO(PTH, PTH6, OUTPUT);    /* CTP_EN , the enable pin of the CTP power */
    OUTPUT_SET(PTH, PTH6);
}

/***********************************************************************************************
*
* @brief    CTP power disenable
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Power_En_Deinit(void)
{
    /* CTP_EN , the enable pin of the CTP power */
    OUTPUT_CLEAR(PTH, PTH6);
    /* CTP _reset, low */
    CTP_Data_Reset_Set(0);
}


#if 0
/***********************************************************************************************
*
* @brief    configure the GPIO
* @param    none
* @return   none
*
************************************************************************************************/
static void CTP_Data_GPIO_Cfg(void)
{
    /* CTP IC reset  set high*/
    CONFIG_PIN_AS_GPIO(PTG,PTG1,OUTPUT); 
    OUTPUT_SET(PTG,PTG1); 
}
#endif
#endif /* CTP_MODULE */
