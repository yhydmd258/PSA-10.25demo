#include "project_cfg.h" 
#ifdef  CTP_MODULE
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
#include <stdio.h>
#include <string.h>
#include "CTP_ctrl.h"
#include "I2C.h"
#include "GPIO.h"
#include "Command/command.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define CTP_MASTER_SEND_TO_SLAVE   0x94    /* the slave is ATMEL 1189T */
#define CTP_MASTER_READ_FROM_SLAVE  0x95    /* the slave is ATMEL 1189T */
#define CTP_IC_SLAVE_ADDRESS    0x4A
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
static void CTP_Data_GPIO_Cfg(void);
static void CTP_Data_Enable_CHG(void);

/**********************************************************************************************
* Global functions
**********************************************************************************************/



/***********************************************************************************************
*
* @brief    deserialize_if_init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Init(void)
{
    I2C_ConfigType  I2C_Config = {{0}};  
	/* start CTP IC*/
	CTP_Data_GPIO_Cfg();
	CTP_Data_Enable_CHG();
    /* Initialize I2C module with interrupt mode */
    I2C_Config.u16Slt = 0x0000;
//    I2C_Config.u16F = 0x0090; /* Baud rate at 100Kbps, MULT=4,  SCL divider=48, 20M/(MULT*divider) */
    I2C_Config.u16F = 0x0010; /* Baud rate at 400Kbps, MULT=1,  SCL divider=48, 20M/(MULT*divider) */
    I2C_Config.sSetting.bMSTEn = 1;
    I2C_Config.sSetting.bIntEn = 0;
    I2C_Config.sSetting.bI2CEn = 1;
//    I2C1_SetCallBack(CTP_I2C1_ISR_CallBack);
    I2C_Init(CTP_IC, &I2C_Config);
	Disable_Interrupt(I2C1_IRQn);

//    I2C_ClearStatus(CTP_IC,I2C_S_IICIF_MASK);
}

/***********************************************************************************************
*
* @brief    deserialize_if_init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Reset_Clear(void)
{
    OUTPUT_CLEAR(PTG,PTG1); 
}
/***********************************************************************************************
*
* @brief    deserialize_if_init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Master_Send(UINT8 *send_data, UINT32 data_size)
{
    if((NULL==send_data) || (0 == data_size))
        return;

    /* send data */
//    I2C_IntDisable(CTP_IC);
    I2C_MasterSendWait(CTP_IC,CTP_IC_SLAVE_ADDRESS<<0x01,&send_data[0],data_size);
//    I2C_IntEnable(CTP_IC);
}
/***********************************************************************************************
*
* @brief    deserialize_if_init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Master_Read(UINT8 *read_data, UINT32 data_size)
{
    if((NULL==read_data) || (0 == data_size))
        return;
    /* read data */
//    I2C_IntDisable(CTP_IC);
    I2C_MasterReadWait(CTP_IC,CTP_IC_SLAVE_ADDRESS<<0x01,read_data,data_size);
//    I2C_IntEnable(CTP_IC);
}
/***********************************************************************************************
*
* @brief    deserialize_if_init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_ISR_CallBack(void)
{
    
}

/***********************************************************************************************
*
* @brief    deserialize_if_init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Cmd_make(TOUCH_COORDINATE_BUF *touch_data)
{
    /*Event codes*/
    #define EVENT_CODE_NO_EVENT     0x00/* No specific event has occured */   
    #define EVENT_CODE_MOVE         0x01/*the touch position has changed*/
    #define EVENT_CODE_UNSUP        0x02/*the touch has just been unsupppressed by 
                                                                        the touch suppression features of other objects*/
    #define EVENT_CODE_SUP          0x03/*the touch has been suppressed by 
                                                                        the touch suppression features of other objects*/
    #define EVENT_CODE_DOWN         0x04/*the touch has just come within range of the sensor*/
    #define EVENT_CODE_UP           0x05/*the touch has just left the range of the sensor*/
    #define EVENT_CODE_UNSUPSUP     0x06/*Both UNSUP and SUP events have occurred(in either order)*/
    #define EVENT_CODE_UNSUPUP      0x07/*Both UNSUP and UP events have occurred(in either order)*/
    #define EVENT_CODE_DOWNSUP      0x08/*Both DOWN and SUP events have occurred(in either order)*/
    #define EVENT_CODE_DOWNUP       0x09/*Both DOWN and UP events have occurred(in either order)*/
    /*touch type*/
    #define TOUCH_TYPE_RESERVED         0x00/*Reserved for future use*/
    #define TOUCH_TYPE_FINGER           0x01/*the touch is considered to be a finger that is contacting the screen*/
    #define TOUCH_TYPE_PASSIVE_STYLUS  0x02/*the touch is a passive stylus*/
    #define TOUCH_TYPE_HOVERING_FINGER 0x04/*the touch is a hovering finger*/
    #define TOUCH_TYPE_GLOVE            0x05/*the touch is a glove touch. Note that touches will be reported as GLOVE events only
                                                                                if the CTRL GLOVERPTEN bit in the Glove Detection T78 object is set to 1;
                                                                                otherwise they will be reported as FINGER events*/
    #define TOUCH_TYPE_LARGE_TOUCH      0x06/*the touch is a suppressed large touch. When the touch Suppression T42 CFG SUPTCHRPTEN
                                                                                bit is set, a touch that would normally be suppressed is reported. When SUPTCHRPTEN is cleared,
                                                                                the suppression method returns to normal behavior.*/
    UINT8   cmd_data[13]={0};
    UINT8   cmd_data_postion=0;
    UINT8   temp_touch[6]={0};
	UINT8   flag=0;
	UINT16   temp_time;

    if( (touch_data->read_pos+1)%COORDINATE_BUF_NUMBER == touch_data->write_pos)
        return;

    /* start to communication with deserializer */
//    CTP_Ctrl_Des_Set_Cds(1);
    while((touch_data->read_pos+1)%COORDINATE_BUF_NUMBER != touch_data->write_pos)
    {
        touch_data->read_pos=(touch_data->read_pos+1)%COORDINATE_BUF_NUMBER;
        memcpy(temp_touch, touch_data->buf[touch_data->read_pos],sizeof(touch_data->buf[touch_data->read_pos]));
        memset(cmd_data, 0x00, sizeof(cmd_data));
        /*       t[0] = 0x79    sync
                        t[1] = 0x6A   Device Adress 
                        t[2] = 0x02   Register or information type
                        t[3]             length of the follow data
                        t[4]             report ID
                        t[5]             touch type
                        t[6]             event codes
                        t[7] = 0x00   LSB of x coordinate
                        t[8] = 0x00   MSB of x coordinate
                        t[9] = 0x00   LSB of y coordinate
                        t[10] = 0x00  MSB of y coordinate
                        t[11] = 0x6A   end of data
                        t[12] = 0x6A   end of data   */
        // make the header of packet
        cmd_data[cmd_data_postion]=COMMAND_SYNC; //D0
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=COMMAND_DEV_ADDR; //D1
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=COMMAND_ID_TOUCH; //D2
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=0x00; //temp set ,after adding message, modify it,D3
        
        /*report ID*/
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=temp_touch[0];   //D4
        /*touch type*/
        cmd_data_postion++;
        switch((temp_touch[1]&0x70)>>0x04)
        {
            case TOUCH_TYPE_FINGER:
                cmd_data[cmd_data_postion] = TOUCH_TYPE_FINGER;   //D5
                break;
            case TOUCH_TYPE_PASSIVE_STYLUS:
                cmd_data[cmd_data_postion] = TOUCH_TYPE_PASSIVE_STYLUS;
                break;
            case TOUCH_TYPE_HOVERING_FINGER:
                cmd_data[cmd_data_postion] = TOUCH_TYPE_HOVERING_FINGER;
                break;
            case TOUCH_TYPE_GLOVE:
                cmd_data[cmd_data_postion] = TOUCH_TYPE_GLOVE;
                break;
            case TOUCH_TYPE_LARGE_TOUCH:
                cmd_data[cmd_data_postion] = TOUCH_TYPE_LARGE_TOUCH;
                break;
            case TOUCH_TYPE_RESERVED:
            default:
                cmd_data[cmd_data_postion] = TOUCH_TYPE_RESERVED;
                break;
        }
        /* event codes */
        cmd_data_postion++;
        switch(temp_touch[1]&0x0F)   //D6
        {
            case EVENT_CODE_DOWN:
                cmd_data[cmd_data_postion] = EVENT_CODE_DOWN;
                break;
            case EVENT_CODE_UP:
                cmd_data[cmd_data_postion] = EVENT_CODE_UP;
                break;
            case EVENT_CODE_MOVE:
                cmd_data[cmd_data_postion] = EVENT_CODE_MOVE;
                break;
            case EVENT_CODE_UNSUP:
                cmd_data[cmd_data_postion] = EVENT_CODE_UNSUP;
                break;
            case EVENT_CODE_SUP:
                cmd_data[cmd_data_postion] = EVENT_CODE_SUP;
                break;
            case EVENT_CODE_UNSUPSUP:
                cmd_data[cmd_data_postion] = EVENT_CODE_UNSUPSUP;
                break;
            case EVENT_CODE_UNSUPUP:
                cmd_data[cmd_data_postion] = EVENT_CODE_UNSUPUP;
                break;
            case EVENT_CODE_DOWNSUP:
                cmd_data[cmd_data_postion] = EVENT_CODE_DOWNSUP;
                break;
            case EVENT_CODE_DOWNUP:
				flag=1;
             //   cmd_data[cmd_data_postion] = EVENT_CODE_DOWNUP;
             	cmd_data[cmd_data_postion] = EVENT_CODE_DOWN;
                break;
            case EVENT_CODE_NO_EVENT:
            default:
                cmd_data[cmd_data_postion] = EVENT_CODE_NO_EVENT;
                break;
        }
        /* x position */
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=temp_touch[4];  //D7
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=temp_touch[5];   //D8
        /* y position */
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=temp_touch[2];   //D9
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=temp_touch[3];	//D10
        cmd_data[3] += 7;
        
        // make the end of packet
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=COMMAND_END_DATA;
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=COMMAND_END_DATA;
        cmd_data[3] += 2;
        
        CTP_Ctrl_Cmd_Send(cmd_data, cmd_data_postion+1);
		if(flag)
			{
			temp_time=0x2FF;/*0.096ms*/
			while(temp_time--);
			cmd_data[6] = EVENT_CODE_UP;
			CTP_Ctrl_Cmd_Send(cmd_data, cmd_data_postion+1);
			flag=0;
			}
//        temp_time=0xC350;/* delay 10ms*/
//        temp_time=0x2FF;/*0.096ms*/
//        while(temp_time--);
    }
    
    /* stop to communication with deserializer */
//    CTP_Ctrl_Des_Set_Cds(0);
}

/***********************************************************************************************
*
* @brief    deserialize_if_init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
UINT32 CTP_Data_Read_INT_Pin(void)
{
    return GPIO_READ_INPUT(PTG,PTG0);
}
/***********************************************************************************************
*
* @brief    CTP_I2C1_GPIO_Cfg() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void CTP_Data_GPIO_Cfg(void)
{
	UINT32 i;

    /* CTP IC reset  set low*/
    CONFIG_PIN_AS_GPIO(PTG,PTG1,OUTPUT); 
    OUTPUT_CLEAR(PTG,PTG1); 
        
    /* CTP power on */
    CONFIG_PIN_AS_GPIO(PTH,PTH6,OUTPUT); 
    OUTPUT_SET(PTH,PTH6);
	     
//i=0x000b0000;/*90.2ms*/
	i=0x0000b000;	
    while(i--);//  delay 

    /* CTP IC reset set high*/
    OUTPUT_SET(PTG,PTG1);

}

/***********************************************************************************************
*
* @brief    deserialize_if_init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void CTP_Data_Enable_CHG(void)
{
    CONFIG_PIN_AS_GPIO(PTG,PTG0,INPUT);
    GPIO_ENABLE_INPUT(PTG,PTG0);
	ENABLE_PULLUP(PTG,PTG0);
}
#endif /* CTP_MODULE */
