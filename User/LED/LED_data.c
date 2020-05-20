#include "project_cfg.h" 
#ifdef  LED_MODULE
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
#include "LED_ctrl.h"
#include "LED_data.h"
#include "LED_I2C.h"
#include "I2C.h"
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
#define LED_I2C		I2C1

/* IC internal error  */
#define FAULT_VOLTAGE_OUTPUT_LOW    0x01
#define FAULT_VIN_OVERCURRENT       0x02
#define FAULT_BOOST_OVERVOLTAGE     0x04
#define FAULT_BOOST_OVERCURENT      0x08
#define FAULT_THERMAL_SHUTDOWN      0x10
#define FAULT_VIN_UNDERVOLTAGE      0x20
#define FAULT_VIN_OVERVOLTAGE       0x40

/* LED error */
#define FAULT_LED_OUT1              0x01  
#define FAULT_LED_OUT2              0x02  
#define FAULT_LED_OUT3              0x04  
#define FAULT_LED_OUT4              0x08  
#define FAULT_LED_SHORT_CIRCUIT     0x10
#define FAULT_LED_OPEN_CIRCUIT      0x20

#define LED_FAULT_ERROR_TOTAL       0x307F
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
void LED_Data_I2C_Init(void)
{
    LED_I2C_Init();
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Data_Cfg_Init(void)
{
#define ADRESS_DATA_GROUP	0x02
#define ADRESS_DATA_SIZE	0x03
    UINT8 address_data_send[ADRESS_DATA_GROUP][ADRESS_DATA_SIZE]={
                                        {0x20,0x02,0x00},{0x28,0x01,0x01}
                                        };
    //UINT8   read_data = 0;
    UINT32   number; 
    for(number=0; number<ADRESS_DATA_GROUP; number++)
        I2C_MasterSendWait(LED_I2C, LED_SLAVE_ADDRESS_WRITE, &(address_data_send[number][0]), ADRESS_DATA_SIZE);
#if 0
	#define ADRESS_DATA_GROUP	0x1E
	#define ADRESS_DATA_SIZE	0x02
    UINT8 address_data_send[ADRESS_DATA_GROUP][ADRESS_DATA_SIZE]={
                                        {0x1A,0x08},{0x1A,0xBA},{0x1A,0xEF},{0x60,0x0F},{0x61,0x54},
                                        {0x62,0xDF},{0x63,0xE0},{0x64,0xDF},{0x65,0x1D},{0x66,0x70},
                                        {0x67,0x77},{0x68,0x77},{0x69,0x71},{0x6A,0x3B},{0x6B,0x00},
                                        {0x6C,0x0A},{0x6D,0x20},{0x6E,0xB0},{0x6F,0x84},{0x70,0xC5},
                                        {0x71,0xF3},{0x72,0xE5},{0x73,0xD0},{0x74,0x35},{0x75,0x06},
                                        {0x76,0xDF},{0x77,0xFF},{0x78,0x3E},{0x19,0x02},{0x19,0x00}
                                        };
    //UINT8   read_data = 0;
    UINT32   number;
    
    for(number=0; number<ADRESS_DATA_GROUP; number++)
        I2C_MasterSendWait(LED_I2C, LED_SLAVE_ADDRESS_WRITE, &(address_data_send[number][0]), ADRESS_DATA_SIZE);
#endif
#if 0            
    // temp code
    LED_I2C_Set_Power_En(0);    /* power off */
    for(number=0; number<0xFF; number++);/* delay */
    LED_I2C_Set_Power_En(1);    /* power on */
#endif    
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Data_Set_PWM(UINT8 pwm_level)
{
    LED_I2C_Set_PWM(pwm_level);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
UINT8 LED_Data_Fault_Status_Read(void)
{
    return LED_I2C_Fault_Status_Read();
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
UINT16 LED_Data_Fault_Msg_Read(void)
{
    #define LED_SLAVE_FAULT_ADDRESS      0x0F
    
    UINT16 fault_msg=0;
    UINT8 u8ErrorStatus;

    I2C_IntDisable(LED_I2C);
    
    /* send start signals to bus */
    I2C_Start(LED_I2C);

    /* send device address to slave */
    u8ErrorStatus = I2C_WriteOneByte(LED_I2C, LED_SLAVE_ADDRESS_WRITE|I2C_WRITE);
    if(u8ErrorStatus == I2C_ERROR_NULL)
    {
        /* if no error occur, received the correct ack from slave
                continue to send data to slave*/
        /* send Register Adress to slave */
        u8ErrorStatus = I2C_WriteOneByte(LED_I2C, LED_SLAVE_FAULT_ADDRESS);
        if(u8ErrorStatus == I2C_ERROR_NULL)
        {
            /* Restart Signal */
            I2C_RepeatStart(LED_I2C);
            /* send device address to slave */
            u8ErrorStatus = I2C_WriteOneByte(LED_I2C, LED_SLAVE_ADDRESS_READ|I2C_READ);
            if(u8ErrorStatus == I2C_ERROR_NULL)
            {
                /* dummy read one byte to switch to Rx mode */
                I2C_ReadOneByte(LED_I2C, (UINT8*)&fault_msg, I2C_SEND_ACK);
                /* read data */
                u8ErrorStatus = I2C_ReadOneByte(LED_I2C, (UINT8*)&fault_msg, I2C_SEND_ACK);
                u8ErrorStatus = I2C_ReadOneByte(LED_I2C, (UINT8*)&fault_msg+1, I2C_SEND_NACK);
            }
        }
    }
    /* send stop signals to bus */
    I2C_Stop(LED_I2C);
     
    I2C_IntEnable(LED_I2C);

    if(!(fault_msg&LED_FAULT_ERROR_TOTAL))
        fault_msg=0;
    
    return fault_msg;
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Data_Fault_Msg_Clear(void)
{
    #define LED_SLAVE_CLEAR_FAULT_ADDRESS   0x11
    #define LED_SLAVE_CLEAR_FAULT_DATA      0x01
    
    UINT8 u8ErrorStatus;

    I2C_IntDisable(LED_I2C);
    
    /* send start signals to bus */
    I2C_Start(LED_I2C);
    
    /* send device address to slave */
    u8ErrorStatus = I2C_WriteOneByte(LED_I2C, LED_SLAVE_ADDRESS_WRITE|I2C_WRITE);
    if(u8ErrorStatus == I2C_ERROR_NULL)
    {
        /* if no error occur, received the correct ack from slave
                continue to send data to slave*/
        /* send Register Adress to slave */
        u8ErrorStatus = I2C_WriteOneByte(LED_I2C, LED_SLAVE_CLEAR_FAULT_ADDRESS);
        if(u8ErrorStatus == I2C_ERROR_NULL)
        {
            u8ErrorStatus = I2C_WriteOneByte(LED_I2C, LED_SLAVE_CLEAR_FAULT_DATA);
        }
    }

    /* send stop signals to bus */
    I2C_Stop(LED_I2C);
     
    I2C_IntEnable(LED_I2C);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Data_Power_Set(UINT8 off_on)
{
    LED_I2C_Set_Power_En(off_on);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
UINT8 LED_Data_Power_En_Check(void)
{
    return LED_I2C_Power_En_Check();
}
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_Data_Fault_Cmd_Make(UINT16 fault_msg)
{    
    #define SEND_LED_FAULT_MSG_SIZE 0x08
    
    UINT8   cmd_data[SEND_LED_FAULT_MSG_SIZE]={0};
    UINT8   fault_data1;
    UINT8   fault_data2;
    UINT16  temp_data;

    if(NULL==cmd_data) 
        return;

    temp_data   = fault_msg&0xFF;
    fault_data1 = temp_data&0xFF;
    temp_data = (fault_msg>>0x08)&0xFF;
    fault_data2 = temp_data&0xFF;
    
    /* make command */
    cmd_data[0] = COMMAND_SYNC;
    cmd_data[1] = COMMAND_DEV_ADDR;
    cmd_data[2] = COMMAND_ID_LED_FAULT;
    /* the size of following data*/
    cmd_data[3] = 0x04;

    cmd_data[4] ^= fault_data1&FAULT_VOLTAGE_OUTPUT_LOW;
    cmd_data[4] ^= fault_data1&FAULT_VIN_OVERCURRENT;
    cmd_data[4] ^= fault_data1&FAULT_BOOST_OVERVOLTAGE;
    cmd_data[4] ^= fault_data1&FAULT_BOOST_OVERCURENT;
    cmd_data[4] ^= fault_data1&FAULT_THERMAL_SHUTDOWN;
    cmd_data[4] ^= fault_data1&FAULT_VIN_UNDERVOLTAGE;
    cmd_data[4] ^= fault_data1&FAULT_VIN_OVERVOLTAGE;
    
    cmd_data[5] ^= fault_data2&FAULT_LED_OUT1;
    cmd_data[5] ^= fault_data2&FAULT_LED_OUT2;
    cmd_data[5] ^= fault_data2&FAULT_LED_OUT3;
    cmd_data[5] ^= fault_data2&FAULT_LED_OUT4;
    cmd_data[5] ^= fault_data2&FAULT_LED_SHORT_CIRCUIT;
    cmd_data[5] ^= fault_data2&FAULT_LED_OPEN_CIRCUIT;
    
    cmd_data[6] = COMMAND_END_DATA;
    cmd_data[7] = COMMAND_END_DATA;

    LED_Ctrl_Cmd_Send(cmd_data, SEND_LED_FAULT_MSG_SIZE);
}
#endif /* LED_MODULE */
